/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MODEL_MODELDATA
#define GRAPEVINE_MODEL_MODELDATA

#include <Grapevine/Math/Matrix.hpp>
#include <Grapevine/Core/String.hpp>
#include "Grapevine/Model/MeshResource.hpp"
#include <stdint.h>
#include <vector>

namespace Grapevine
{
    class ModelFactory;
    class Mesh;

    struct AttributeType
    {
        enum Enum
        {
            Position    = 1,
            Normal      = 2,
            Texcoord    = 3,
            Tangent     = 4,
            Bitangent   = 5,
            Custom      = 6
        };
    };

    class ModelData
    {
    public:
        ~ModelData();
        ModelData();
        void I();
        void convertToMesh(MeshResource& mesh, const std::vector<std::pair<AttributeType::Enum, int> > &attribs);
        void convertMaterials(MeshResource& target);
        void calculateNormals();

    private:
        struct Face
        {
            uint32_t a;
            uint32_t b;
            uint32_t c;
            uint16_t faceFlags;
        };

        struct Vertex
        {
            Vector3 v;
            Vector3 n;
            Vector2 t;
            Vector3 nt;
            Vector3 nb;
        };

        struct Object
        {
            Object(const std::string &name):
                name(name),
                boundingSphereRadius(0.0f)
            {
            }

            Object():
                name(""),
                boundingSphereRadius(0.0f)
            {
            }

            std::string name;
            std::vector<Vertex> vertices;
            std::vector<Face> faces;
            Matrix4 localTransform_;
            float boundingSphereRadius;
            int materialID;
        };

        struct Material
        {
            Material():
                colorMapName(""),
                normalMapName(""),
                specularFactor(0.0f),
                specularGlossiness(0.0f),
                reflectionFactor(0.0f)
            {
            }

            std::string colorMapName;
            std::string normalMapName;
            float specularFactor;
            float specularGlossiness;
            float reflectionFactor;
            int materialID;
        };

        void appendObjectToMesh(const Object &object, MeshResource& mesh, int p_pos, int n_pos, int t_pos, int nt_pos, int nb_pos);
        //String name_;
        std::vector<Vertex> vertices_;
        std::vector<Face> faces_;
        Vector3 maxBoundingVector_;
        Vector3 minBoundingVector_;
        std::vector<Object> objects_;
        std::vector<Material> materials_;

        friend class ModelFactory;
        friend class MeshResource;
        friend class ModelResource;
        friend class MaterialResource;
    };
}

#endif

