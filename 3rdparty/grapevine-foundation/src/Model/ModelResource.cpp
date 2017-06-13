/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Model/ModelResource.hpp"
#include "ModelFactory.hpp"
#ifdef ANDROID
    #include <Grapevine/Core/Exception.hpp>
#endif

namespace Grapevine
{
    ModelResource::ModelResource(std::string const& filename):
        Resource(filename),
        modelData_(ModelFactory::loadModelData(filename))
    {
        // Create meshes
        for (size_t i = 0; i < modelData_->objects_.size(); i++)
        {
            ModelData* modelData = new ModelData;
            modelData->objects_.push_back(modelData_->objects_[i]);
            meshes_.push_back(MeshAsset(filename + std::string("_meshid_%d", i), modelData));

            materialOfMesh_.push_back(modelData_->objects_[i].materialID);
            Log::D("linked mesh %d to material %d", i, modelData_->objects_[i].materialID);
        }
        // Create materials
        for (size_t i = 0; i < modelData_->materials_.size(); i++)
        {
            ModelData * modelData = new ModelData;
            modelData->materials_.push_back(modelData_->materials_[i]);
            materials_.push_back(MaterialAsset(filename + std::string("_material_%d", i), modelData));
        }

        minBoundingVector_ = modelData_->minBoundingVector_;
        maxBoundingVector_ = modelData_->maxBoundingVector_;
    }

    ModelResource::~ModelResource()
    {
    }

    unsigned int ModelResource::meshCount()
    {
        return meshes_.size();
    }

    unsigned int ModelResource::materialCount()
    {
        return materials_.size();
    }

    MeshAsset ModelResource::mesh(unsigned int index)
    {
        if(index >= meshCount())
        {
            throw Exception("ModelResource::mesh() invalid mesh index");
        }
        return meshes_[index];
    }

    MaterialAsset ModelResource::materialOfMeshIndex(unsigned int index)
    {
        if(index >= materialOfMesh_.size())
        {
            throw Exception("ModelResource::materialOfMeshIndex() invalid mesh index");
        }
        unsigned int materialIndex = materialOfMesh_[index];
        if(materialIndex >= materialCount())
        {
            throw Exception("ModelResource::materialOfMeshIndex() invalid material index");
        }
        return materials_[materialIndex];
    }

    Vector3 ModelResource::minBoundingVector()
    {
        return minBoundingVector_;
    }

    Vector3 ModelResource::maxBoundingVector()
    {
        return maxBoundingVector_;
    }
}
