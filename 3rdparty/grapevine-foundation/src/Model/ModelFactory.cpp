/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <Grapevine/Resource/ResourceManagerPrivate.hpp>
#include "ModelFactory.hpp"
#include "Grapevine/Model/MeshResource.hpp"
#include <Grapevine/Core/String.hpp>
#include <Grapevine/Core/Util.hpp>
#include <Grapevine/Core/Exception.hpp>
#include <Grapevine/Resource/File.hpp>
#include "Barrel.hpp"
#include <float.h> // FLT_MIN, FLT_MAX
#include <fstream>
#include <sstream>
#include <memory>
#ifdef ANDROID
    #include <Grapevine/Core/Exception.hpp>
#endif

namespace Grapevine
{
    ModelData* ModelFactory::loadModelData(std::string const& filename)
    {
        ModelData* r = NULL;

        // Load data to non-GPU specific intermediate format
        if (File::hasEnding(filename, std::string(".3ds")) ||
            File::hasEnding(filename, std::string(".3DS")))
        {
            r = load3ds(filename);
        }
        else if (File::hasEnding(filename, std::string(".barrel")))
        {
            File tmp(filename);
            r = loadBarrel(tmp);
        }
        else
        {
            Log::E("file format in file %s has no support.", filename.c_str());
            throw Exception("file format not supported");
        }

        LOGD("Materials loaded:%d", r->materials_.size());
        return r;
    }

    ModelData* ModelFactory::createRectangle(float width, float height, float textureUsageU, float textureUsageV, bool flipX, bool flipY)
    {
        ModelData* r = new ModelData;
        r->objects_.push_back(ModelData::Object());
        ModelData::Object* object = &r->objects_.back();

        // Coordinates
        object->vertices.resize(4);
        Vector3 normal = Vector3(0.0f, 0.0f, 1.0f);

        if (flipX)
        {
            width = -width;
        }

        if (flipY)
        {
            height = -height;
        }

        // Point 1, lower left
        unsigned int index = 0;
        object->vertices[index].v.x(-width/2.0f);
        object->vertices[index].v.y(-height/2.0f);
        object->vertices[index].v.z(0.0f);

        object->vertices[index].t.x(0.0f);
        object->vertices[index].t.y(0.0f);

        object->vertices[index].n = normal;

        // Point 2, upper left
        index = 1;
        object->vertices[index].v.x(-width/2.0f);
        object->vertices[index].v.y(height/2.0f);
        object->vertices[index].v.z(0.0f);

        object->vertices[index].t.x(0.0f);
        object->vertices[index].t.y(textureUsageV);

        object->vertices[index].n = normal;

        // Point 3, upper right
        index = 2;
        object->vertices[index].v.x(width/2.0f);
        object->vertices[index].v.y(height/2.0f);
        object->vertices[index].v.z(0.0f);

        object->vertices[index].t.x(textureUsageU);
        object->vertices[index].t.y(textureUsageV);

        object->vertices[index].n = normal;

        // Point 4, lower right
        index = 3;
        object->vertices[index].v.x(width/2.0f);
        object->vertices[index].v.y(-height/2.0f);
        object->vertices[index].v.z(0.0f);

        object->vertices[index].t.x(textureUsageU);
        object->vertices[index].t.y(0.0f);

        object->vertices[index].n = normal;

        // Faces
        object->faces.resize(2);

        index = 0;
        object->faces[index].a = 0;
        object->faces[index].b = 1;
        object->faces[index].c = 3;

        index = 1;
        object->faces[index].a = 1;
        object->faces[index].b = 2;
        object->faces[index].c = 3;

        // Bounding sphere radius
        object->boundingSphereRadius = Vector2(width/2.0f, height/2.0f).length();

        return r;
    }

    ModelData* ModelFactory::createRectangle(float width, float height, Vector2 const& startPoint, Vector2 const& rectSize, bool flipX, bool flipY)
    {
        return createRectangle(width, height, Vector2(0.0f, 0.0f), startPoint, rectSize, flipX, flipY);
    }

    ModelData* ModelFactory::createRectangle(float width, float height, Vector2 const& positionOffset, Vector2 const& startPoint, Vector2 const& rectSize, bool flipX, bool flipY)
    {
        ModelData* r = new ModelData;
        r->objects_.push_back(ModelData::Object());
        ModelData::Object* object = &r->objects_.back();

        // Coordinates
        object->vertices.resize(4);
        Vector3 normal = Vector3(0.0f, 0.0f, 1.0f);

        if (flipX)
        {
            width = -width;
        }

        if (flipY)
        {
            height = -height;
        }

        // Point 1, lower left
        unsigned int index = 0;
        object->vertices[index].v.x(positionOffset.x() - width/2.0f);
        object->vertices[index].v.y(positionOffset.y() - height/2.0f);
        object->vertices[index].v.z(0.0f);

        object->vertices[index].t.x(startPoint.x());
        object->vertices[index].t.y(startPoint.y());

        object->vertices[index].n = normal;

        // Point 2, upper left
        index = 1;
        object->vertices[index].v.x(positionOffset.x() - width/2.0f);
        object->vertices[index].v.y(positionOffset.y() + height/2.0f);
        object->vertices[index].v.z(0.0f);

        object->vertices[index].t.x(startPoint.x());
        object->vertices[index].t.y(startPoint.y() + rectSize.y());

        object->vertices[index].n = normal;

        // Point 3, upper right
        index = 2;
        object->vertices[index].v.x(positionOffset.x() + width/2.0f);
        object->vertices[index].v.y(positionOffset.y() + height/2.0f);
        object->vertices[index].v.z(0.0f);

        object->vertices[index].t.x(startPoint.x() + rectSize.x());
        object->vertices[index].t.y(startPoint.y() + rectSize.y());

        object->vertices[index].n = normal;

        // Point 4, lower right
        index = 3;
        object->vertices[index].v.x(positionOffset.x() + width/2.0f);
        object->vertices[index].v.y(positionOffset.y() - height/2.0f);
        object->vertices[index].v.z(0.0f);

        object->vertices[index].t.x(startPoint.x() + rectSize.x());
        object->vertices[index].t.y(startPoint.y());

        object->vertices[index].n = normal;

        // Faces
        object->faces.resize(2);

        index = 0;
        object->faces[index].a = 0;
        object->faces[index].b = 1;
        object->faces[index].c = 3;

        index = 1;
        object->faces[index].a = 1;
        object->faces[index].b = 2;
        object->faces[index].c = 3;

        // Bounding sphere radius
        object->boundingSphereRadius = Vector2(width/2.0f + positionOffset.x(), height/2.0f + positionOffset.y()).length();

        return r;
    }


    ModelData* ModelFactory::createModelData(std::vector<Vector3> const& position, std::vector<Vector2> const& uv, std::vector<Vector3> const& normal, std::vector<uint16_t> const &indices)
    {
        ModelData* r = new ModelData;
        r->objects_.push_back(ModelData::Object());
        ModelData::Object* object = &r->objects_.back();

        unsigned int vertexCount = position.size();

        // Coordinates
        object->vertices.resize(vertexCount);

        float maxDist = 0.f;

        for (unsigned int i=0; i<vertexCount; ++i)
        {
            object->vertices[i].v = position[i];
            object->vertices[i].n = normal.empty() ? Vector3(0.f, 0.f, 1.f) : normal[i];
            object->vertices[i].t = uv.empty() ? Vector2(0.f, 0.f) : uv[i];

            float dist = position[i].length();
            if (dist > maxDist)
            {
                dist = maxDist;
            }
        }

        if (indices.empty())
        {
            object->faces.resize(vertexCount / 3);

            for (unsigned int i=0; i<vertexCount; i+=3)
            {
                object->faces[i].a = i + 0;
                object->faces[i].b = i + 1;
                object->faces[i].c = i + 2;
            }
        }
        else
        {
            unsigned int indexCount = indices.size();

            object->faces.resize(indexCount / 3);

            for (unsigned int i=0; i<indexCount/3; ++i)
            {
                object->faces[i].a = indices[i * 3 + 0];
                object->faces[i].b = indices[i * 3 + 1];
                object->faces[i].c = indices[i * 3 + 2];
            }
        }

        // Bounding sphere radius
        object->boundingSphereRadius = maxDist;

        return r;
    }

    ModelData* ModelFactory::createFromBuffer(char* data, unsigned int byteSize)
    {
        float maxDist = 0.f;
        ModelData* r = new ModelData;
        r->objects_.push_back(ModelData::Object());
        ModelData::Object* object = &r->objects_.back();
        //LOGD("ModelFactory bytesize: %d", byteSize);

        // Vertices
        float* dataFloat = (float*)(data);
        unsigned int floatCount = byteSize / sizeof(float);
        unsigned int vertexCount = byteSize / 8 / sizeof(float);
        //LOGD("ModelFactory vertexCount: %d", vertexCount);
        object->vertices.resize(vertexCount);
        for(unsigned int i = 0; i < vertexCount; i++)
        {
            unsigned int iF = i * 8;
            Vector3 vec(dataFloat[iF + 0], dataFloat[iF + 1], dataFloat[iF + 2]);
            object->vertices[i].v = vec;
            object->vertices[i].n = Vector3(dataFloat[iF + 3], dataFloat[iF + 4], dataFloat[iF + 5]);
            object->vertices[i].t = Vector2(dataFloat[iF + 6], dataFloat[iF + 7]);

            maxDist = std::max(maxDist, vec.length());
        }

        // Faces
        unsigned int faceCount = vertexCount / 3;
        unsigned int vertexIndex = 0;
        object->faces.resize(faceCount);
        for(unsigned int i = 0; i < faceCount; i++)
        {
            object->faces[i].a = vertexIndex++;
            object->faces[i].b = vertexIndex++;
            object->faces[i].c = vertexIndex++;
        }

        object->boundingSphereRadius = maxDist;

        return r;
    }

    /**
     * Computes the bounding box for a ModelData.
     *
     * @param object the Model object
     */
    void ModelFactory::computeBoundingBox(ModelData* model, ModelData::Object* object)
    {
        float xMin(FLT_MAX);
        float xMax(FLT_MIN);
        float yMin(FLT_MAX);
        float yMax(FLT_MIN);
        float zMin(FLT_MAX);
        float zMax(FLT_MIN);

        for (unsigned int i = 0; i < object->vertices.size(); ++i)
        {
            Vector3 current = object->vertices.at(i).v;
            if (current.x() < xMin)
                xMin = current.x();
            if (current.x() > xMax)
                xMax = current.x();

            if (current.y() < yMin)
                yMin = current.y();
            if (current.y() > yMax)
                yMax = current.y();

            if (current.z() < zMin)
                zMin = current.z();
            if (current.z() > zMax)
                zMax = current.z();
        }
        model->maxBoundingVector_.x(xMax);
        model->maxBoundingVector_.y(yMax);
        model->maxBoundingVector_.z(zMax);
        model->minBoundingVector_.x(xMin);
        model->minBoundingVector_.y(yMin);
        model->minBoundingVector_.z(zMin);
    }

    /**
     * Load a model from a 3DS file.
     *
     * @param filename the name of the file
     *
     * @return ModelData created from the loaded data
     */
    ModelData* ModelFactory::load3ds(std::string const& file)
    {
        ModelData* modelData = new ModelData;
        ModelData::Object *object(0);

        File inputFile(file);

        // Scan the whole file
        while (inputFile.endOfFile() == false)
        {
            uint16_t chunkID;
            uint32_t chunkLength;

            // Read the chunk header
            inputFile.read(reinterpret_cast<char*>(&chunkID), 2);

            // Read the length of the chunk
            inputFile.read(reinterpret_cast<char*>(&chunkLength), 4);

            switch (chunkID)
            {
                // Main chunk, contains all the other chunks
                // id: 4d4d
                // length: 0 + sub chunks
                case 0x4d4d:
                    break;

                // 3D Editor chunk, objects layout info
                // id: 3d3d (hex)
                // length: 0 + sub chunks
                case 0x3d3d:
                    break;

                // object block, info for each object
                // id: 4000 (hex)
                // length: len(object name) + sub chunks
                case 0x4000:
                {
                    std::stringstream ss;
                    unsigned char c = 1;

                    for (int i = 0; i < 20 && c != '\0'; i++)
                    {
                        inputFile.read(reinterpret_cast<char*>(&c), 1);
                        ss << c;
                    }

                    modelData->objects_.push_back(ModelData::Object(ss.str()));
                    object = &modelData->objects_.back();
                }
                break;

                // Triangular mesh, contains chunks for 3d mesh info
                // id: 4100 (hex)
                // length: 0 + sub chunks
                case 0x4100:
                    break;

                // Vertices list
                // id: 4110 (hex)
                // length: 1 x unsigned short (number of vertices)
                //     + 3 x float (vertex coordinates) x (number of vertices)
                //     + sub chunks
                case 0x4110:
                {
                    float boundingSphereRadius = 0.0f;

                    uint16_t qty;
                    inputFile.read(reinterpret_cast<char*>(&qty), sizeof(uint16_t));
                    object->vertices.resize(qty);
                    for (uint16_t i = 0; i < qty; i++)
                    {
                        float f[3];
                        inputFile.read(reinterpret_cast<char*>(f), sizeof(float) * 3);
                        object->vertices[i].v.x(f[0]);
                        object->vertices[i].v.y(f[1]);
                        object->vertices[i].v.z(f[2]);
                        if (boundingSphereRadius < object->vertices[i].v.length())
                        {
                            boundingSphereRadius = object->vertices[i].v.length();
                        }
                    }

                    object->boundingSphereRadius = boundingSphereRadius;
                }
                break;

                // Polygons (faces) list
                // id: 4120 (hex)
                // length: 1 x unsigned short (number of polygons)
                //     + 3 x unsigned short (polygon points) x (number of polygons)
                //     + sub chunks
                case 0x4120:
                {
                    uint16_t qty;
                    inputFile.read(reinterpret_cast<char*>(&qty), sizeof(uint16_t));
                    object->faces.resize(qty);
                    for (uint16_t i = 0; i < qty; i++)
                    {
                        inputFile.read(reinterpret_cast<char*>(&object->faces[i].a), sizeof(uint16_t));
                        inputFile.read(reinterpret_cast<char*>(&object->faces[i].b), sizeof(uint16_t));
                        inputFile.read(reinterpret_cast<char*>(&object->faces[i].c), sizeof(uint16_t));
                        inputFile.read(reinterpret_cast<char*>(&object->faces[i].faceFlags), sizeof(uint16_t));
                    }
                }
                break;

                // Vertices list
                // id: 4140 (hex)
                // length: 1 x unsigned short (mapping points amount)
                //     + 2 x float (mapping coordinates) x (mapping points amount)
                //     + sub chunks
                case 0x4140:
                {
                    uint16_t qty;
                    inputFile.read(reinterpret_cast<char*>(&qty), sizeof(uint16_t));
                    for (uint16_t i = 0; i < qty; i++)
                    {
                        float f[2];
                        inputFile.read(reinterpret_cast<char*>(f), sizeof(float) * 2);
                        object->vertices[i].t.x(f[0]);
                        object->vertices[i].t.y(f[1]);
                    }
                }
                break;

                // Unknow chunks
                // Skip chunks that we don't use. Length used to set file pointer to
                // the level of next chunk.
                default:
                     inputFile.seek(chunkLength - 6);
            }
        }
        computeBoundingBox(modelData, object);
        modelData->calculateNormals();

        return modelData;
    }

    ModelData* ModelFactory::loadBarrel(File& file)
    {
        ModelData* modelData = new ModelData;

        while (file.endOfFile() == false)
        {
            std::string blockHeader = loadBarrelBlockHeader(file);
            //LOGD("%s got", blockHeader.c_str());

            if (blockHeader == Barrel::FileTagStart)
            {
                //LOGD("Barrel::FileTagStart");
            }
            else if (blockHeader == Barrel::FileTagEnd)
            {
                //LOGD("Barrel::FileTagEnd");
            }
            else if (blockHeader == Barrel::FileTagMesh)
            {
                //LOGD("Barrel::FileTagMesh");

                modelData->objects_.push_back(ModelData::Object());
                ModelData::Object* object = &modelData->objects_.back();
                readBarrelMesh(file, object);
                computeBoundingBox(modelData, object);
            }
            else if (blockHeader == Barrel::FileTagMaterial)
            {
                //LOGD("Barrel::FileTagMaterial");
                modelData->materials_.push_back(ModelData::Material());
                ModelData::Material* material = &modelData->materials_.back();
                readBarrelMaterial(file, material);
            }
            else
            {
                //LOGE("ModelFactory::loadBarrel(), block header \"%s\"", blockHeader.c_str());
                throw Exception("ModelFactory::loadBarrel(), unkown blockHeader");
            }
        }

        return modelData;
    }

    void ModelFactory::readBarrelMesh(File& inputFile, ModelData::Object* object)
    {
        // Read name
        uint16_t nameLength;
        inputFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));

        char name[257];
        for (unsigned int i = 0; i < 257; i++)
        {
            name[i] = (char)0;
        }
        for (uint8_t i = 0; i < nameLength + 1; i++)  // TODO polish byte-by-byte reading
        {
            inputFile.read(reinterpret_cast<char*>(&name[i]), 1);
        }

        // Material ID linked to this mesh
        int materialID;
        inputFile.read(reinterpret_cast<char*>(&materialID), sizeof(materialID));
        object->materialID = materialID;

        // 16 * 32bit: local matrix
        float localMatrix[16];
        for (unsigned int i = 0; i < 16; i++)
        {
            inputFile.read(reinterpret_cast<char*>(&localMatrix[i]), sizeof(localMatrix[i]));
        }
        object->localTransform_ = Grapevine::Matrix4(&localMatrix[0]);
/*
        LOGD(" - local transformation matrix:");
        object->localTransform_.print();
*/

        // 32bit unsigned int: amount of control points
        uint32_t pointsAmount = 0;
        inputFile.read(reinterpret_cast<char*>(&pointsAmount), sizeof(pointsAmount));

        // 8bit unsigned int: uv channels
        uint8_t amountOfUvChannels = 0;
        inputFile.read(reinterpret_cast<char*>(&amountOfUvChannels), sizeof(amountOfUvChannels));

        // 8bit unsigned int: normal channels
        uint8_t amountOfNormalChannels = 0;
        inputFile.read(reinterpret_cast<char*>(&amountOfNormalChannels), sizeof(amountOfNormalChannels));

        // Load vertices
        object->vertices.resize(pointsAmount);

        for (uint32_t i = 0; i < pointsAmount; i++)
        {
            //Log::D("i:%d", i);

            // Coordinates
            float xyz[3];
            inputFile.read(reinterpret_cast<char*>(&xyz), 3 * sizeof(float));
            object->vertices[i].v.x(xyz[0]);
            object->vertices[i].v.y(xyz[1]);
            object->vertices[i].v.z(xyz[2]);

            // TODO: boundingSphereRadius could be saved to barrel
            float distanceFromObjectOrigo = object->vertices[i].v.length();
            if (distanceFromObjectOrigo > object->boundingSphereRadius)
            {
                object->boundingSphereRadius = distanceFromObjectOrigo;
            }

            // UV channels
            for (uint8_t uvChannel = 0; uvChannel < amountOfUvChannels; uvChannel++)
            {
                float uv[2];
                inputFile.read(reinterpret_cast<char*>(&uv), 2 * sizeof(float));
                object->vertices[i].t.x(uv[0]);
                object->vertices[i].t.y(uv[1]);
            }

            // Normal channels
            for (uint8_t normalChannel = 0; normalChannel < amountOfNormalChannels; normalChannel++)
            {
                float xyz[3];
                inputFile.read(reinterpret_cast<char*>(&xyz), 3 * sizeof(float));
                object->vertices[i].n.x(xyz[0]);
                object->vertices[i].n.y(xyz[1]);
                object->vertices[i].n.z(xyz[2]);
            }

            // Zero the tangent and bitangent
            object->vertices[i].nt = Vector::origo;
            object->vertices[i].nb = Vector::origo;
        }

        // 32bit unsigned int: amount of polyhons
        uint32_t polygonAmount = 0;
        inputFile.read(reinterpret_cast<char*>(&polygonAmount), sizeof(polygonAmount));

/*
        LOGD("# Loading mesh\n"
            " - points: %d\n"
            " - uv channels: %d\n"
            " - normal channels: %d\n"
            " - polygon count: %d",
            pointsAmount, amountOfUvChannels, amountOfNormalChannels, polygonAmount);
*/
        object->faces.resize(polygonAmount);
        //Log::D("faces resized");

        std::map<uint32_t, uint32_t> vertexUsageCount;
        for (uint32_t i = 0; i < polygonAmount; i++)
        {
            //Log::D("i:%d", i);

            // Indices
            uint32_t p012[3];
            inputFile.read(reinterpret_cast<char*>(&p012), 3 * sizeof(uint32_t));
            object->faces[i].a = p012[0];
            object->faces[i].b = p012[1];
            object->faces[i].c = p012[2];

            vertexUsageCount[p012[0]]++;
            vertexUsageCount[p012[1]]++;
            vertexUsageCount[p012[2]]++;

            // Calculate tangent
            {
                // Shortcuts for vertices
                Vector3 v0 = object->vertices[object->faces[i].a].v;
                Vector3 v1 = object->vertices[object->faces[i].b].v;
                Vector3 v2 = object->vertices[object->faces[i].c].v;

                // Shortcuts for UVs
                Vector2 uv0 = object->vertices[object->faces[i].a].t;
                Vector2 uv1 = object->vertices[object->faces[i].b].t;
                Vector2 uv2 = object->vertices[object->faces[i].c].t;

                // Edges of the triangle : postion delta
                Vector3 deltaPos1 = v1-v0;
                Vector3 deltaPos2 = v2-v0;

                // UV delta
                Vector2 deltaUV1 = uv1-uv0;
                Vector2 deltaUV2 = uv2-uv0;

                float r = 1.0f / (deltaUV1.x() * deltaUV2.y() - deltaUV1.y() * deltaUV2.x());
                Vector3 tangent = (deltaPos1 * deltaUV2.y() - deltaPos2 * deltaUV1.y())*r;
                Vector3 bitangent = (deltaPos2 * deltaUV1.x() - deltaPos1 * deltaUV2.x())*r;

                object->vertices[object->faces[i].a].nt += tangent;
                object->vertices[object->faces[i].b].nt += tangent;
                object->vertices[object->faces[i].c].nt += tangent;
                object->vertices[object->faces[i].a].nb += bitangent;
                object->vertices[object->faces[i].b].nb += bitangent;
                object->vertices[object->faces[i].c].nb += bitangent;
            }
        }

        // Process tangent data
        for (uint32_t i = 0; i < pointsAmount; i++)
        {
            object->vertices[i].nt /= (float)vertexUsageCount[i];
            object->vertices[i].nb /= (float)vertexUsageCount[i];
        }
        // LOGD("mesh loaded! Bounding sphere: %f", object->boundingSphereRadius);
    }

    void ModelFactory::readBarrelMaterial(File& inputFile, ModelData::Material* material)
    {
        // read ID
        int materialID;
        inputFile.read(reinterpret_cast<char*>(&materialID), sizeof(materialID));

        std::string colorMapName = read8BitLengthAnd0To255Chars(inputFile);
        std::string normalMapName = read8BitLengthAnd0To255Chars(inputFile);
        float specularFactor = readFloat(inputFile);
        float specularGlossiness = readFloat(inputFile);
        float reflectionFactor = readFloat(inputFile);
/*
        LOGD("colorMapName %s", colorMapName.c_str());
        LOGD("normalMapName %s", normalMapName.c_str());
        LOGD("specularFactor %.3f", specularFactor);
        LOGD("specularGlossiness %.3f", specularGlossiness);
        LOGD("reflectionFactor %.3f", reflectionFactor);
*/
        material->materialID = materialID;
        material->colorMapName = colorMapName;
        material->normalMapName = normalMapName;
        material->specularFactor = specularFactor;
        material->specularGlossiness = specularGlossiness;
        material->reflectionFactor = reflectionFactor;
    }

    std::string ModelFactory::read8BitLengthAnd0To255Chars(File& inputFile)
    {
        uint8_t nameLength;
        inputFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));

        char name[257];
        for (unsigned int i = 0; i < 257; i++)
        {
            name[i] = (char)0;
        }

        for (uint8_t i = 0; i < nameLength; i++)  // TODO polish byte-by-byte reading
        {
            inputFile.read(reinterpret_cast<char*>(&name[i]), 1);
            //LOGD("char: '%c'", name[i]);
        }

        return std::string(&name[0]);
    }

    float ModelFactory::readFloat(File& inputFile)
    {
        float r;
        inputFile.read(reinterpret_cast<char*>(&r), sizeof(r));
        return r;
    }

    std::string ModelFactory::loadBarrelBlockHeader(File& inputFile)
    {
        std::string header = "";
        unsigned int charsRead = 0;

        while (charsRead < Barrel::MaximumBlockHeaderLength)
        {
            char tmp = '?';
            inputFile.read(reinterpret_cast<char*>(&tmp), sizeof(tmp));
            //LOGD("barrel header char: '%c'", tmp);
            header += tmp;
            charsRead++;
            if (charsRead == 1 && tmp != '[')
            {
                LOGE("Block does not start with '[', char was:%c", tmp);
                throw Exception("ModelFactory::loadBarrelBlockHeader(), Block does not start with '['");
            }

            if (tmp == ']') // block header end
            {
                break;
            }
        }

        return header;
    }
}
