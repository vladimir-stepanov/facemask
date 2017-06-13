/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MODEL_MODELFACTORY
#define GRAPEVINE_MODEL_MODELFACTORY

#include "ModelData.hpp"
#include "Grapevine/Model/MeshResource.hpp"
#include <Grapevine/Resource/File.hpp>
#include <string>
#include <vector>

namespace Grapevine
{
    class Mesh;

    class ModelFactory
    {
    public:
        static ModelData* loadModelData(std::string const& file);

        /* Creates rectangle data with UV and normals, center of the object is (0.0, 0.0, 0.0) */
        static ModelData* createRectangle(float width, float height, float textureUsageU, float textureUsageV, bool flipX, bool flipY);
        static ModelData* createRectangle(float width, float height, Vector2 const& startPoint, Vector2 const& rectSize, bool flipX, bool flipY);
        static ModelData* createRectangle(float width, float height, Vector2 const& positionOffset, Vector2 const& startPoint, Vector2 const& rectSize, bool flipX, bool flipY);
        static ModelData* cube();

        /* create model from user data */
        static ModelData* createModelData(std::vector<Vector3> const& position, std::vector<Vector2> const& uv, std::vector<Vector3> const& normal, std::vector<uint16_t> const &indices);

        // position, normal, uv: 3+3+2 4byte floats per vertex triangle array
        static ModelData* createFromBuffer(char* data, unsigned int byteSize);

    private:
        ModelFactory();
        ~ModelFactory();

        static void computeBoundingBox(ModelData*, ModelData::Object*);
        static ModelData* load3ds(std::string const& file);
        static ModelData* loadBarrel(File& file);
        static void readBarrelMesh(File& file, ModelData::Object* object);
        static void readBarrelMaterial(File& file, ModelData::Material* material);
        static std::string read8BitLengthAnd0To255Chars(File& file);
        static float readFloat(File& file);
        static std::string loadBarrelBlockHeader(File& file);
    };
}

#endif
