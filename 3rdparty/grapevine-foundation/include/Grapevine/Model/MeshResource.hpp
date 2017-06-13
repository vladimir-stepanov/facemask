/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MODEL_MESHRESOURCE
#define GRAPEVINE_MODEL_MESHRESOURCE

#include <Grapevine/Math/Box.hpp>
#include <Grapevine/Math/Vector.hpp>
#include "Grapevine/Resource/Resource.hpp"
#include "Grapevine/Material/MaterialResource.hpp"
#include "Grapevine/Model/MeshAsset.hpp"
#include "Grapevine/Model/DynamicMesh.hpp"
#include <vector>
#include <stdint.h>

namespace Grapevine
{
    class MeshResource: public Resource
    {
    public:
        struct VBOUpdateMethod
        {
            enum Enum
            {
                Map,
                SubData
            };
        };

        struct VBOUsage
        {
            enum Enum
            {
                Static,
                Stream,
                Dynamic
            };
        };

        struct Type
        {
            enum Enum
            {
                Mesh,
                Rectangle,
                DynamicMesh
            };
        };

        MeshResource(std::string const& file);
        MeshResource(std::string const& name, class ModelData* modelData);
        MeshResource(float width, float height, float textureUsageU, float textureUsageV, bool flipX, bool flipY);
        MeshResource(float width, float height, Vector2 const& startPoint, Vector2 const& rectSize, bool flipX, bool flipY);
        MeshResource(float width, float height, Vector2 const& positionOffset, Vector2 const& startPoint, Vector2 const& rectSize, bool flipX, bool flipY);
        MeshResource(std::vector<Vector3> const& position, std::vector<Vector2> const& uv, std::vector<Vector3> const& normal, std::vector<uint16_t> const &indices);
        MeshResource(DynamicMesh const& dynamicMesh);
        MeshResource(char* data, unsigned int byteSize);
        virtual ~MeshResource();
        void createGPUSpecificData();
        float boundingSphereRadius();
        void setBoundingSphereRadius(float);
        Box const& boundingBox();
        void setLocalMatrix(Matrix4 const& localMatrix);
        Matrix4 const& localMatrix();
        void render();
        void renderDynamicMesh(int positionLocation, int normalLocation, int textureUVLocation);
        void setVertexFormat(const std::vector<int>& format);
        void setAttributeLocations(const std::vector<int>& locations);
        void setAttribute(unsigned int pos, const Vector2 &v, std::vector<float> *vertex = 0);
        void setAttribute(unsigned int pos, const Vector3 &v, std::vector<float> *vertex = 0);
        void setAttribute(unsigned int pos, const Vector4 &v, std::vector<float> *vertex = 0);
        void nextVertex();
        std::vector< std::vector<float> >& vertices();
        void VBOUpdateMethod(VBOUpdateMethod::Enum method);
        void VBOUsage(VBOUsage::Enum usage);
        void interleave(bool interleave);
        void reset();
        void buildArray();
        void buildVBO();
        void updateArray(const std::vector<std::pair<size_t, size_t> >& ranges);
        void updateVBO(const std::vector<std::pair<size_t, size_t> >& ranges);
        void deleteArray();
        void deleteVBO();
        void renderArray();
        MaterialResource* materialResource();
        void setMaterialResource(MaterialResource* materialResource);
        Type::Enum type();

        // Only set for Rectangle type
        float width() const;

        // Only set for Rectangle type
        float height() const;

        // vbos
        const std::vector<unsigned int> &vbos() const;

        static void purgeStaleVBOs();
    private:

        static std::vector<unsigned int> staleVBOs_;
        void renderVBO() const;
        bool checkAttribute(unsigned int pos, int dim);
        std::vector<float>& ensureVertex();
        void updateSingleArray(const std::vector<std::pair<size_t, size_t> >& ranges, size_t n, size_t nfloats, size_t offset);
        void updateSingleVBO(const std::vector<std::pair<size_t, size_t> >& ranges, size_t n, size_t nfloats);

        // vertexFormat_ is vector of <dimension, amount of values> pairs. Examples:
        //  -Vector2 with 3 values would be <2, 3> pair,
        //  -One vertex could have different pairs-> vertex takes space like next:
        //   <3, 3>, <2, 3>, <2, 3>     = 9 + 6 + 6    = 21 (*sizeof(float))
        std::vector<std::pair<int, int> > vertexFormat_;
        std::vector<int> attributeLocations_;
        Matrix4 localTransform_;
        int vertexSize_;
        std::vector<std::vector<float> > vertices_;
        std::vector<float*> vertexArrays_;
        std::vector<unsigned int> vbos_;
        std::vector<float*> attributeData_;
        int vertexStride_;
        bool interleave_;
        VBOUpdateMethod::Enum VBOUpdateMethod_;
        VBOUsage::Enum VBOUsage_;
        bool vboBuilt_;
        class ModelData* modelData_;
        MaterialResource* materialResource_;
        Type::Enum type_;
        float width_;
        float height_;
        float boundingSphereRadius_;
        Box boundingBox_;
        DynamicMesh dynamicMesh_;
    };
}

#endif
