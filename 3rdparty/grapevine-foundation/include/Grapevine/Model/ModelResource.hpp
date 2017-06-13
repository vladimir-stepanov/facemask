/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MODEL_MODELRESOURCE
#define GRAPEVINE_MODEL_MODELRESOURCE

#include "Grapevine/Resource/Resource.hpp"
#include <Grapevine/Model/MeshAsset.hpp>
#include <Grapevine/Material/MaterialAsset.hpp>
#include <string>
#include <vector>

namespace Grapevine
{
    class ModelResource: public Resource
    {
    public:
        ModelResource(std::string const& filename);
        ~ModelResource();
        unsigned int meshCount();
        unsigned int materialCount();
        MeshAsset mesh(unsigned int index);
        MaterialAsset materialOfMeshIndex(unsigned int index);
        Vector3 minBoundingVector();
        Vector3 maxBoundingVector();

    private:
        class ModelData* modelData_;
        std::vector<MeshAsset> meshes_;
        std::vector<MaterialAsset> materials_;
        std::vector<unsigned int> materialOfMesh_;
        Vector3 minBoundingVector_;
        Vector3 maxBoundingVector_;
    };
}

#endif
