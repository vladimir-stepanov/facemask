/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "DynamicMeshPrivate.hpp"
#include <Grapevine/Core/UniquePointer.hpp>

namespace Grapevine
{
    DynamicMeshPrivate::DynamicMeshPrivate(std::vector<std::vector<ControlPoint> > const& rowsColsGrid):
        rowsColsGrid_(rowsColsGrid),
        vertexCount_(0),
        vbo_(0)
    {
        checkGrid();
    }

    DynamicMeshPrivate::~DynamicMeshPrivate()
    {
        if (vbo_ != 0)
        {
            glDeleteBuffers(1, &vbo_);
        }
    }

    void DynamicMeshPrivate::checkGrid()
    {
        // Check that given vector vector array is really a grid
        {
            if (rowsColsGrid_.size() < 2)
            {
                throw Exception("Grid must have at least 2 cols!");
            }

            unsigned int firstRowItemCount = rowsColsGrid_[0].size();
            if (firstRowItemCount < 2)
            {
                throw Exception("Grid must have at least 2 rows!");
            }

            unsigned int row = 0;
            for(std::vector<std::vector<ControlPoint> >::const_iterator i = rowsColsGrid_.begin();
                i != rowsColsGrid_.end();
                i++)
            {
                if ((*i).size() != firstRowItemCount)
                {
                    LOGE("Your grid row %d has unmatching number of items", row);
                    throw Exception("All grid rows should have same amount of cols.");
                }
            }
        }
    }

    void DynamicMeshPrivate::update()
    {
        createVBO();
    }

    void DynamicMeshPrivate::createVBO()
    {
        unsigned int rows = rowsColsGrid_.size();
        unsigned int cols = rowsColsGrid_[0].size();

        vertexCount_ = 2 * (rows - 1) * (cols - 1) * 3; // (2 triangles / square) * (squares) * (3 vertices per triangle)
        UniquePointer<GLfloat> vertices(new GLfloat[vertexCount_ * 8], true); //  8 values per vertex
        //LOGD("created amount of vertices: %d", vertexCount_);

        // Create triangle data
        GLfloat* data = vertices.pointer(); // just for tmp use
        unsigned int dataIndex = 0; // index to vertices

        for (unsigned int upperRow = 0, lowerRow = 1; lowerRow < rows; upperRow++, lowerRow++)
        {
            for (unsigned int leftCol = 0, rightCol = 1; rightCol < cols; leftCol++, rightCol++)
            {
                /*
                 * Rectangle we create has two triangles:
                 *
                 * 1 ___ 2
                 *  |  /|
                 *  | / |
                 *  |/__|
                 * 0     3
                 *
                 */

                // Location
                std::vector<Vector3> locations;
                locations.push_back(rowsColsGrid_[lowerRow][leftCol].location());
                locations.push_back(rowsColsGrid_[upperRow][leftCol].location());
                locations.push_back(rowsColsGrid_[upperRow][rightCol].location());
                locations.push_back(rowsColsGrid_[lowerRow][rightCol].location());

                // Normal
                std::vector<Vector3> normals;
                normals.push_back(Vector::transform(Vector::zAxis, rowsColsGrid_[lowerRow][leftCol].rotation()));
                normals.push_back(Vector::transform(Vector::zAxis, rowsColsGrid_[upperRow][leftCol].rotation()));
                normals.push_back(Vector::transform(Vector::zAxis, rowsColsGrid_[upperRow][rightCol].rotation()));
                normals.push_back(Vector::transform(Vector::zAxis, rowsColsGrid_[lowerRow][rightCol].rotation()));

                // Texture UV
                std::vector<Vector2> uvs;
                uvs.push_back(Vector2(
                    (float)leftCol / (float)(cols - 1),
                    1.0f - (float)lowerRow / (float)(rows - 1)));
                uvs.push_back(Vector2(
                    (float)leftCol / (float)(cols - 1),
                    1.0f - (float)upperRow / (float)(rows - 1)));
                uvs.push_back(Vector2(
                    (float)rightCol / (float)(cols - 1),
                    1.0f - (float)upperRow / (float)(rows - 1)));
                uvs.push_back(Vector2(
                    (float)rightCol / (float)(cols - 1),
                    1.0f - (float)lowerRow / (float)(rows - 1)));

                // triangle 0 - 1 - 2 and 0 - 2 - 3
                std::vector<unsigned int> indices;
                indices.push_back(0);
                indices.push_back(1);
                indices.push_back(2);
                indices.push_back(0);
                indices.push_back(2);
                indices.push_back(3);

                for (unsigned int i = 0; i < indices.size(); i++)
                {
                    //LOGD("index: %d", i);

                    // position
                    data[dataIndex++] = locations[indices[i]].x();
                    data[dataIndex++] = locations[indices[i]].y();
                    data[dataIndex++] = locations[indices[i]].z();
                    //LOGD("   p.x: %f", locations[indices[i]].x());
                    //LOGD("   p.y: %f", locations[indices[i]].y());
                    //LOGD("   p.z: %f", locations[indices[i]].z());

                    // normal
                    data[dataIndex++] = normals[indices[i]].x();
                    data[dataIndex++] = normals[indices[i]].y();
                    data[dataIndex++] = normals[indices[i]].z();
                    //LOGD("   n.x: %f", normals[indices[i]].x());
                    //LOGD("   n.y: %f", normals[indices[i]].y());
                    //LOGD("   n.z: %f", normals[indices[i]].z());

                    // texture
                    data[dataIndex++] = uvs[indices[i]].x();
                    data[dataIndex++] = uvs[indices[i]].y();
                    //LOGD("   uv.x: %f", uvs[indices[i]].x());
                    //LOGD("   uv.y: %f", uvs[indices[i]].y());
                }
            }
        }


        if(vbo_ == 0)
        {
            // Create new vbo
            glGenBuffers(1, &vbo_);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_);
            glBufferData(GL_ARRAY_BUFFER, vertexCount_ * 8 * sizeof(GLfloat), data, GL_DYNAMIC_DRAW);
        }
        else
        {
            // If old vbo exists we write over it
            glBindBuffer(GL_ARRAY_BUFFER, vbo_);
            glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount_ * 8 * sizeof(GLfloat), data);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void DynamicMeshPrivate::draw(GLint positionLocation, GLint normalLocation, GLint textureUVLocation)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);

        // enable shader locations
        glEnableVertexAttribArray(positionLocation);
        glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 8* sizeof(GLfloat),  (GLvoid*)(0* sizeof(GLfloat)));
        if(normalLocation >= 0)
        {
            glEnableVertexAttribArray(normalLocation);
            glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, 8* sizeof(GLfloat),  (GLvoid*)(3* sizeof(GLfloat)));
        }
        if(textureUVLocation >= 0)
        {
            glEnableVertexAttribArray(textureUVLocation);
            glVertexAttribPointer(textureUVLocation, 2, GL_FLOAT, GL_FALSE, 8* sizeof(GLfloat),  (GLvoid*)(6* sizeof(GLfloat)));
        }

        // draw
        glDrawArrays(GL_TRIANGLES, 0, vertexCount_);

        // disable shader locations
        glDisableVertexAttribArray(positionLocation);
        if(normalLocation >= 0)
        {
            glDisableVertexAttribArray(normalLocation);
        }
        if(textureUVLocation >= 0)
        {
            glDisableVertexAttribArray(textureUVLocation);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    float DynamicMeshPrivate::boundingSphereRadius() const
    {
        float r = 0.0f;
        unsigned int rows = rowsColsGrid_.size();
        unsigned int cols = rowsColsGrid_[0].size();
        for (unsigned int x = 0; x < cols; x++)
        {
            for (unsigned int y = 0; y < rows; y++)
            {
                /*float tmp = rowsColsGrid_[x][y].location().length();
                if (r < tmp)
                {
                    r = tmp;
                }*/
            }
        }
        return r;
    }
}
