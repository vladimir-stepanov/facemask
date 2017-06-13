/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Model/MeshResource.hpp"
#include "ModelFactory.hpp"
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/GPU/GL.hpp>
#include <Grapevine/Model/MeshResource.hpp>
#include "DynamicMeshPrivate.hpp"

namespace Grapevine
{
    MeshResource::MeshResource(const std::string& filename):
        Resource(filename),
        vertexSize_(0),
        interleave_(false),
        VBOUpdateMethod_(VBOUpdateMethod::Map),
        VBOUsage_(VBOUsage::Static),
        vboBuilt_(false),
        modelData_(ModelFactory::loadModelData(filename)),
        materialResource_(NULL),
        type_(MeshResource::Type::Mesh),
        width_(0.0f),
        height_(0.0f),
        boundingSphereRadius_(0.0f),
        boundingBox_()
    {
        // Set bounding sphere for this mesh resource
        for (std::vector<ModelData::Object>::iterator i = modelData_->objects_.begin(); i != modelData_->objects_.end(); i++)
        {
            if ((*i).boundingSphereRadius > boundingSphereRadius_)
            {
                boundingSphereRadius_ = (*i).boundingSphereRadius;
            }
        }

        boundingBox_ = Box(Vector3(-boundingSphereRadius_), Vector3(boundingSphereRadius_));
    }

    MeshResource::MeshResource(const std::string& name, class ModelData* modelData):
        Resource(name),
        localTransform_(),
        vertexSize_(0),
        interleave_(false),
        VBOUpdateMethod_(VBOUpdateMethod::Map),
        VBOUsage_(VBOUsage::Static),
        vboBuilt_(false),
        modelData_(modelData),
        materialResource_(NULL),
        type_(MeshResource::Type::Mesh),
        width_(0.0f),
        height_(0.0f),
        boundingSphereRadius_(0.0f),
        boundingBox_()
    {
        // Set bounding sphere for this mesh resource
        for (std::vector<ModelData::Object>::iterator i = modelData_->objects_.begin(); i != modelData_->objects_.end(); i++)
        {
            if ((*i).boundingSphereRadius > boundingSphereRadius_)
            {
                boundingSphereRadius_ = (*i).boundingSphereRadius;
            }
        }

        boundingBox_ = Box(Vector3(-boundingSphereRadius_), Vector3(boundingSphereRadius_));
    }

    MeshResource::MeshResource(float width, float height, Vector2 const& startPoint, Vector2 const& rectSize, bool flipX, bool flipY):
        Resource(std::string()),
        localTransform_(),
        vertexSize_(0),
        interleave_(false),
        VBOUpdateMethod_(VBOUpdateMethod::Map),
        VBOUsage_(VBOUsage::Static),
        vboBuilt_(false),
        modelData_(ModelFactory::createRectangle(width, height, startPoint, rectSize, flipX, flipY)),
        materialResource_(NULL),
        type_(MeshResource::Type::Rectangle),
        width_(width),
        height_(height),
        boundingSphereRadius_(sqrt(width* width + height* height)/2.f),
        boundingBox_(Vector3(-width/2.f, -height/2.f, 0.f), Vector3(width/2.f, height/2.f, 0.f))
    {
    }

    MeshResource::MeshResource(float width, float height, Vector2 const& positionOffset, Vector2 const& startPoint, Vector2 const& rectSize, bool flipX, bool flipY):
        Resource(std::string()),
        localTransform_(),
        vertexSize_(0),
        interleave_(false),
        VBOUpdateMethod_(VBOUpdateMethod::Map),
        VBOUsage_(VBOUsage::Static),
        vboBuilt_(false),
        modelData_(ModelFactory::createRectangle(width, height, positionOffset, startPoint, rectSize, flipX, flipY)),
        materialResource_(NULL),
        type_(MeshResource::Type::Rectangle),
        width_(width),
        height_(height),
        boundingSphereRadius_(sqrt(width* width + height* height)/2.f),
        boundingBox_(Vector3(-width/2.f, -height/2.f, 0.f), Vector3(width/2.f, height/2.f, 0.f))
    {
    }

    MeshResource::MeshResource(float width, float height, float textureUsageU, float textureUsageV, bool flipX, bool flipY):
        Resource(std::string()),
        localTransform_(),
        vertexSize_(0),
        interleave_(false),
        VBOUpdateMethod_(VBOUpdateMethod::Map),
        VBOUsage_(VBOUsage::Static),
        vboBuilt_(false),
        modelData_(ModelFactory::createRectangle(width, height, textureUsageU, textureUsageV, flipX, flipY)),
        materialResource_(NULL),
        type_(MeshResource::Type::Rectangle),
        width_(width),
        height_(height),
        boundingSphereRadius_(sqrt(width* width + height* height)/2.f),
        boundingBox_(Vector3(-width/2.f, -height/2.f, 0.f), Vector3(width/2.f, height/2.f, 0.f))
    {
    }

    MeshResource::MeshResource(std::vector<Vector3> const& position, std::vector<Vector2> const& uv, std::vector<Vector3> const& normal, std::vector<uint16_t> const &indices):
        Resource(std::string()),
        localTransform_(),
        vertexSize_(0),
        interleave_(false),
        VBOUpdateMethod_(VBOUpdateMethod::Map),
        VBOUsage_(VBOUsage::Static),
        vboBuilt_(false),
        modelData_(ModelFactory::createModelData(position, uv, normal, indices)),
        materialResource_(NULL),
        type_(MeshResource::Type::Mesh),
        width_(0.0f),
        height_(0.0f),
        boundingSphereRadius_(0.0f),
        boundingBox_()
    {
        // Set bounding sphere for this mesh resource
        for (std::vector<ModelData::Object>::iterator i = modelData_->objects_.begin(); i != modelData_->objects_.end(); i++)
        {
            if ((*i).boundingSphereRadius > boundingSphereRadius_)
            {
                boundingSphereRadius_ = (*i).boundingSphereRadius;
            }
        }

        boundingBox_ = Box(Vector3(-boundingSphereRadius_), Vector3(boundingSphereRadius_));
    }

    MeshResource::MeshResource(DynamicMesh const& dynamicMesh):
        Resource(std::string()),
        localTransform_(),
        vertexSize_(0),
        interleave_(false),
        VBOUpdateMethod_(VBOUpdateMethod::Map),
        VBOUsage_(VBOUsage::Static),
        vboBuilt_(false),
        modelData_(NULL),
        materialResource_(NULL),
        type_(MeshResource::Type::DynamicMesh),
        width_(0),
        height_(0),
        boundingSphereRadius_(dynamicMesh.boundingSphereRadius()),
        boundingBox_(Vector3(-boundingSphereRadius_), Vector3(boundingSphereRadius_)),
        dynamicMesh_(dynamicMesh)
    {
    }

    MeshResource::MeshResource(char* data, unsigned int byteSize):
        Resource(std::string()),
        localTransform_(),
        vertexSize_(0),
        interleave_(false),
        VBOUpdateMethod_(VBOUpdateMethod::Map),
        VBOUsage_(VBOUsage::Static),
        vboBuilt_(false),
        modelData_(ModelFactory::createFromBuffer(data, byteSize)),
        materialResource_(NULL),
        type_(MeshResource::Type::Mesh),
        width_(0.0f),
        height_(0.0f),
        boundingSphereRadius_(0.0f)
    {
        // Set bounding sphere for this mesh resource
        for (std::vector<ModelData::Object>::iterator i = modelData_->objects_.begin(); i != modelData_->objects_.end(); i++)
        {
            if ((*i).boundingSphereRadius > boundingSphereRadius_)
            {
                boundingSphereRadius_ = (*i).boundingSphereRadius;
            }
        }

        boundingBox_ = Box(Vector3(-boundingSphereRadius_), Vector3(boundingSphereRadius_));
    }

    MeshResource::~MeshResource()
    {
        reset();
        if (materialResource_ != NULL)
        {
            materialResource_->decrement();
        }
        if (modelData_ != NULL)
        {
            delete modelData_;
        }
    }

    void MeshResource::createGPUSpecificData()
    {
        if (vboBuilt_ == false && type_ != MeshResource::Type::DynamicMesh)
        {
            interleave(true); // interleaved arrays.

            std::vector< std::pair<AttributeType::Enum, int> > attributeLocations;
            attributeLocations.push_back(std::pair<AttributeType::Enum, int>(AttributeType::Position, 3));
            attributeLocations.push_back(std::pair<AttributeType::Enum, int>(AttributeType::Normal, 3));
            attributeLocations.push_back(std::pair<AttributeType::Enum, int>(AttributeType::Texcoord, 2));
            modelData_->convertToMesh(*this, attributeLocations);
            modelData_->convertMaterials(*this);

            delete modelData_;
            modelData_ = NULL;

            buildVBO();
            vboBuilt_ = true;
        }
        else if (type_ == MeshResource::Type::DynamicMesh)
        {
            // update data if needed
            dynamicMesh_.update();
        }
    }

    float MeshResource::boundingSphereRadius()
    {
        // if dynamic mesh, we need to recalculate the value
        if(type_ == MeshResource::Type::DynamicMesh)
        {
            boundingSphereRadius_ = dynamicMesh_.boundingSphereRadius();
        }
        return boundingSphereRadius_;
    }

    void MeshResource::setBoundingSphereRadius(float r)
    {
        boundingSphereRadius_ = r;
    }

    Box const& MeshResource::boundingBox()
    {
        return boundingBox_;
    }

    void MeshResource::setLocalMatrix(Matrix4 const& localMatrix)
    {
        localTransform_ = localMatrix;
    }

    Matrix4 const& MeshResource::localMatrix()
    {
        return localTransform_;
    }

    void MeshResource::render()
    {
        if (modelData_ == NULL && type_ != MeshResource::Type::DynamicMesh)
        {
            renderVBO();
        }
    }

    void MeshResource::renderDynamicMesh(int positionLocation, int normalLocation, int textureUVLocation)
    {
        dynamicMesh_.private_->draw(positionLocation, normalLocation, textureUVLocation);
    }

    /**
     * Sets the vertex format for this mesh.
     *
     * The format consists of a vector of integers, each
     * specifying the size in floats of each vertex attribute.
     *
     * e.g. {4, 3, 2} => 3 attributes vec4, vec3, vec2
     */
    void MeshResource::setVertexFormat(const std::vector<int> &format)
    {
        int pos = 0;
        vertexFormat_.clear();

        for (std::vector<int>::const_iterator iter = format.begin(); iter != format.end(); iter++)
        {
            int n = *iter;
            vertexFormat_.push_back(std::pair<int,int>(n, pos));
            pos += n;
        }

        vertexSize_ = pos;
    }

    /**
     * Sets the attribute locations.
     *
     * These are the locations used in glEnableVertexAttribArray()
     * and other related functions.
     */
    void MeshResource::setAttributeLocations(const std::vector<int> &locations)
    {
        attributeLocations_ = locations;
    }

    /**
     * Checks that given attribute is existing and that it is dimensionally the
     * same as given one.
     *
     * @param pos the position/index of the attribute to check
     * @param dim the size of the attribute (in #floats)
     * @returns whether the check succeeded
     */
    bool MeshResource::checkAttribute(unsigned int pos, int dim)
    {
        if (pos > vertexFormat_.size())
        {
            LOGE("Trying to set non-existent attribute!");
            return false;
        }

        if (vertexFormat_[pos].first != dim)
        {
            LOGE("Trying to set attribute with value of invalid type!");
            return false;
        }

        return true;
    }

    /**
     * Ensures that we have a vertex to process.
     *
     * @return the vertex to process
     */
    std::vector<float> &MeshResource::ensureVertex()
    {
        if (vertices_.empty())
        {
            nextVertex();
        }

        return vertices_.back();
    }

    /**
     * Sets the value of an attribute in the current vertex.
     *
     * The pos parameter refers to the position of the attribute
     * as specified indirectly when setting the format using
     * setVertexFormat(). e.g. 0 = first attribute, 1 = second
     * etc
     */
    void MeshResource::setAttribute(unsigned int pos, const Vector2 &v,
        std::vector<float> *vertex)
    {
        if (!checkAttribute(pos, 2))
        {
            return;
        }

        std::vector<float> &vtx = !vertex ? ensureVertex() : *vertex;

        int offset = vertexFormat_[pos].second;

        vtx[offset] = v.x();
        vtx[offset + 1] = v.y();
    }

    void MeshResource::setAttribute(unsigned int pos, const Vector3 &v,
        std::vector<float> *vertex)
    {
        if (!checkAttribute(pos, 3))
        {
            return;
        }

        std::vector<float> &vtx = !vertex ? ensureVertex() : *vertex;

        int offset = vertexFormat_[pos].second;

        vtx[offset] = v.x();
        vtx[offset + 1] = v.y();
        vtx[offset + 2] = v.z();
    }

    void MeshResource::setAttribute(unsigned int pos, const Vector4 &v,
        std::vector<float> *vertex)
    {
        if (!checkAttribute(pos, 4))
        {
            return;
        }

        std::vector<float> &vtx = !vertex ? ensureVertex() : *vertex;

        int offset = vertexFormat_[pos].second;

        vtx[offset] = v.x();
        vtx[offset + 1] = v.y();
        vtx[offset + 2] = v.z();
        vtx[offset + 3] = v.w();
    }

    /**
     * Adds a new vertex to the list and makes it current.
     */
    void MeshResource::nextVertex()
    {
        vertices_.push_back(std::vector<float>(vertexSize_));
    }

    /**
     * Gets the mesh vertices.
     *
     * You should use the ::setAttribute() method to manipulate
     * the vertex data.
     *
     * You shouldn't resize the vector (change the number of vertices)
     * manually. Use ::nextVertex() instead.
     */
    std::vector<std::vector<float> >& MeshResource::vertices()
    {
        return vertices_;
    }

    /**
     * Sets the VBO update method.
     *
     * The default value is VBOUpdateMethodMap.
     */
    void MeshResource::VBOUpdateMethod(VBOUpdateMethod::Enum method)
    {
        VBOUpdateMethod_ = method;
    }

    /**
     * Sets the VBO usage hint.
     *
     * The usage hint takes effect in the next call to ::buildVBO().
     *
     * The default value is VBOUsageStatic.
     */
    void MeshResource::VBOUsage(VBOUsage::Enum usage)
    {
        VBOUsage_ = usage;
    }

    /**
     * Sets the vertex attribute interleaving mode.
     *
     * If true the vertex attributes are going to be interleaved in a single
     * buffer. Otherwise they will be separated into different buffers (one
     * per attribute).
     *
     * Interleaving mode takes effect in the next call to ::buildArray() or
     * ::buildVBO().
     *
     * @param interleave whether to interleave
     */
    void MeshResource::interleave(bool interleave)
    {
        interleave_ = interleave;
    }

    /**
     * Resets a Mesh object to its initial, empty state.
     */
    void MeshResource::reset()
    {
        deleteArray();
        deleteVBO();

        vertices_.clear();
        vertexFormat_.clear();
        attributeLocations_.clear();
        attributeData_.clear();
        vertexSize_ = 0;
        vertexStride_ = 0;
    }

    /**
     * Builds a vertex array containing the mesh vertex data.
     *
     * The way the vertex array is constructed is affected by the current
     * interleave value, which can set using ::interleave().
     */
    void MeshResource::buildArray()
    {
        int nvertices = vertices_.size();

        if (!interleave_)
        {
            // Create an array for each attribute
            for (std::vector<std::pair<int, int> >::const_iterator ai =
                vertexFormat_.begin(); ai != vertexFormat_.end(); ai++)
            {

                float *array = new float[nvertices * ai->first];
                float *cur = array;

                // Fill array
                for (std::vector<std::vector<float> >::const_iterator vi =
                    vertices_.begin(); vi != vertices_.end(); vi++)
                {
                    for (int i = 0; i < ai->first; i++)
                    {
                        *cur++ = (*vi)[ai->second + i];
                    }
                }

                vertexArrays_.push_back(array);
                attributeData_.push_back(array);
            }
            vertexStride_ = 0;
        }
        else
        {
            float *array = new float[nvertices * vertexSize_];
            float *cur = array;

            for (std::vector<std::vector<float> >::const_iterator vi =
                vertices_.begin(); vi != vertices_.end(); vi++)
            {

                // Fill array
                for (int i = 0; i < vertexSize_; i++)
                {
                    *cur++ = (*vi)[i];
                }
            }

            for (size_t i = 0; i < vertexFormat_.size(); i++)
            {
                attributeData_.push_back(array + vertexFormat_[i].second);
            }

            vertexArrays_.push_back(array);
            vertexStride_ = vertexSize_ * sizeof(float);
        }
    }

    /**
     * Builds a vertex buffer object containing the mesh vertex data.
     *
     * The way the VBO is constructed is affected by the current interleave
     * value (::interleave()) and the vbo usage hint (::VBOUsage()).
     */
    void MeshResource::buildVBO()
    {
        deleteArray();
        buildArray();

        int nvertices = vertices_.size();

        attributeData_.clear();

        GLenum bufferUsage = GL_STREAM_DRAW;
        if (VBOUsage_ == VBOUsage::Stream)
        {
            bufferUsage = GL_STREAM_DRAW;
        }
        else if (VBOUsage_ == VBOUsage::Dynamic)
        {
            bufferUsage = GL_DYNAMIC_DRAW;
        }
        else if (VBOUsage_ == VBOUsage::Static)
        {
            bufferUsage = GL_STATIC_DRAW;
        }

        if (!interleave_)
        {
            // Create VBO for each attribute
            for (std::vector<std::pair<int, int> >::const_iterator ai =
                vertexFormat_.begin(); ai != vertexFormat_.end(); ai++)
            {
                float *data = vertexArrays_[ai - vertexFormat_.begin()];

                GLuint vbo;
                glGenBuffers(1, &vbo);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                glBufferData(GL_ARRAY_BUFFER, nvertices * ai->first * sizeof(float), data, bufferUsage);

                vbos_.push_back(vbo);
                attributeData_.push_back(0);
            }

            vertexStride_ = 0;
        }
        else
        {
            // Create a single VBO to store all attribute data
            GLuint vbo;

            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);

            glBufferData(GL_ARRAY_BUFFER, nvertices * vertexSize_ * sizeof(float), vertexArrays_[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            for (size_t i = 0; i < vertexFormat_.size(); i++)
            {
                attributeData_.push_back(reinterpret_cast<float *>(sizeof(float) * vertexFormat_[i].second));
                vbos_.push_back(vbo);
            }
            vertexStride_ = vertexSize_ * sizeof(float);
        }

        deleteArray();
    }

    /**
     * Updates ranges of a single vertex array.
     *
     * @param ranges the ranges of vertices to update
     * @param n the index of the vertex array to update
     * @param nfloats how many floats to update for each vertex
     * @param offset the offset (in floats) in the vertex data to start reading from
     */
    void MeshResource::updateSingleArray(const std::vector<std::pair<size_t, size_t> >& ranges,
        size_t n, size_t nfloats, size_t offset)
    {
        float *array(vertexArrays_[n]);

        /* Update supplied ranges */
        for (std::vector<std::pair<size_t, size_t> >::const_iterator ri = ranges.begin();
             ri != ranges.end(); ri++)
        {

            /* Update the current range from the vertex data */
            float *dest(array + nfloats * ri->first);
            for (size_t n = ri->first; n <= ri->second; n++)
            {
                for (size_t i = 0; i < nfloats; i++)
                {
                    *dest++ = vertices_[n][offset + i];
                }
            }
        }
    }

    /**
     * Updates ranges of the vertex arrays.
     *
     * @param ranges the ranges of vertices to update
     */
    void MeshResource::updateArray(const std::vector<std::pair<size_t, size_t> >& ranges)
    {
        /* If we don't have arrays to update, create them */
        if (vertexArrays_.empty())
        {
            buildArray();
            return;
        }

        if (!interleave_)
        {
            for (size_t i = 0; i < vertexArrays_.size(); i++)
            {
                updateSingleArray(ranges, i, vertexFormat_[i].first,
                    vertexFormat_[i].second);
            }
        }
        else
        {
            updateSingleArray(ranges, 0, vertexSize_, 0);
        }
    }

    /**
     * Updates ranges of a single VBO.
     *
     * This method use either glMapBuffer or glBufferSubData to perform
     * the update. The used method can be set with ::VBOUpdateMethod().
     *
     * @param ranges the ranges of vertices to update
     * @param n the index of the vbo to update
     * @param nfloats how many floats to update for each vertex
     */
    void MeshResource::updateSingleVBO(const std::vector<std::pair<size_t, size_t> >& ranges,
        size_t n, size_t nfloats)
    {
        float *src_start(vertexArrays_[n]);
        float *dest_start(0);

        glBindBuffer(GL_ARRAY_BUFFER, vbos_[n]);

        if (VBOUpdateMethod_ == VBOUpdateMethod::Map)
        {
            dest_start = reinterpret_cast<float *>(
                GLExtensions::MapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
        }

        // Update ranges
        for (std::vector<std::pair<size_t, size_t> >::const_iterator iter =
            ranges.begin(); iter != ranges.end(); iter++)
        {

            float *src(src_start + nfloats * iter->first);
            float *src_end(src_start + nfloats * (iter->second + 1));

            if (VBOUpdateMethod_ == VBOUpdateMethod::Map)
            {
                float *dest(dest_start + nfloats * iter->first);
                std::copy(src, src_end, dest);
            }
            else if (VBOUpdateMethod_ == VBOUpdateMethod::SubData)
            {
                glBufferSubData(GL_ARRAY_BUFFER, nfloats * iter->first *
                    sizeof(float), (src_end - src) * sizeof(float), src);
            }
        }

        if (VBOUpdateMethod_ == VBOUpdateMethod::Map)
        {
            GLExtensions::UnmapBuffer(GL_ARRAY_BUFFER);
        }
    }

    /**
     * Updates ranges of the VBOs.
     *
     * @param ranges the ranges of vertices to update
     */
    void MeshResource::updateVBO(const std::vector<std::pair<size_t, size_t> >& ranges)
    {
        // If we don't have VBOs to update, create them
        if (vbos_.empty())
        {
            buildVBO();
            return;
        }

        updateArray(ranges);

        if (!interleave_)
        {
            for (size_t i = 0; i < vbos_.size(); i++)
            {
                updateSingleVBO(ranges, i, vertexFormat_[i].first);
            }
        }
        else
        {
            updateSingleVBO(ranges, 0, vertexSize_);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }


    /**
     * Deletes all resources associated with built vertex arrays.
     */
    void MeshResource::deleteArray()
    {
        for (size_t i = 0; i < vertexArrays_.size(); i++)
        {
            delete [] vertexArrays_[i];
        }

        vertexArrays_.clear();
    }

    /**
     * Deletes all resources associated with built VBOs.
     */
    void MeshResource::deleteVBO()
    {
        staleVBOs_.insert(staleVBOs_.end(), vbos_.begin(), vbos_.end());
        if (!interleave_) {
            staleVBOs_.insert(staleVBOs_.end(), vbos_.begin(), vbos_.end());
        } else if (vbos_.size()) {
            staleVBOs_.push_back(vbos_[0]);
        }
        vbos_.clear();
    }

    /**
     * Renders a mesh using vertex arrays.
     *
     * Vertex arrays must have been initialized with ::buildArray().
     */
    void MeshResource::renderArray()
    {
        for (size_t i = 0; i < vertexFormat_.size(); i++)
        {
            if(attributeLocations_[i] < 0) continue;
            glEnableVertexAttribArray(attributeLocations_[i]);
            glVertexAttribPointer(attributeLocations_[i], vertexFormat_[i].first,
                GL_FLOAT, GL_FALSE, vertexStride_, attributeData_[i]);
        }

        glDrawArrays(GL_TRIANGLES, 0, vertices_.size());

        for (size_t i = 0; i < vertexFormat_.size(); i++)
        {
            if(attributeLocations_[i] >= 0)
            {
                glDisableVertexAttribArray(attributeLocations_[i]);
            }
        }
    }

    /**
     * Renders a mesh using vertex buffer objects.
     *
     * Vertex buffer objects must have been initialized with ::buildVBO().
     */
    void MeshResource::renderVBO() const
    {
        for (size_t i = 0; i < vertexFormat_.size(); i++){

            if(attributeLocations_[i] < 0) continue;

            glEnableVertexAttribArray(attributeLocations_[i]);
            glBindBuffer(GL_ARRAY_BUFFER, vbos_[i]);
            glVertexAttribPointer(attributeLocations_[i], vertexFormat_[i].first,
                GL_FLOAT, GL_FALSE, vertexStride_, attributeData_[i]);
        }

        glDrawArrays(GL_TRIANGLES, 0, vertices_.size());

        for (size_t i = 0; i < vertexFormat_ .size(); i++){

            if(attributeLocations_[i] >= 0){
                glDisableVertexAttribArray(attributeLocations_[i]);
            }
        }
    }

    MaterialResource* MeshResource::materialResource()
    {
        return materialResource_;
    }

    void MeshResource::setMaterialResource(MaterialResource* materialResource)
    {
        materialResource_ = materialResource;
        materialResource_->increment();
    }

    MeshResource::Type::Enum MeshResource::type()
    {
        return type_;
    }

    float MeshResource::width() const
    {
        return width_;
    }

    float MeshResource::height() const
    {
        return height_;
    }

    void MeshResource::purgeStaleVBOs()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        if(!staleVBOs_.empty())
        {
            glDeleteBuffers(staleVBOs_.size(), &staleVBOs_[0]);
            staleVBOs_.clear();
        }
    }

    const std::vector<unsigned int> &MeshResource::vbos() const
    {
        return vbos_;
    }

    std::vector<unsigned int> MeshResource::staleVBOs_;

}
