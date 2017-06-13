/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/SceneExtensions/ModelComponentPrivate.hpp"
#include <Grapevine/Animation/AnimationManager.hpp>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            ModelComponentPrivate::ModelComponentPrivate():
                AbstractComponentPrivateTemplate<ModelComponentPrivate>("ModelComponent"),
                opacity_(1.0f)
            {
            }

            ModelComponentPrivate::~ModelComponentPrivate()
            {
            }

            void ModelComponentPrivate::addMeshMaterialPair(MeshAsset& mesh, MaterialAsset& material)
            {
                meshMaterialPairs_.push_back(ModelComponent::MeshMaterialPair(mesh, material));
            }

            std::vector<ModelComponent::MeshMaterialPair>& ModelComponentPrivate::meshMaterialPairs()
            {
                return meshMaterialPairs_;
            }
        }
    }
}
