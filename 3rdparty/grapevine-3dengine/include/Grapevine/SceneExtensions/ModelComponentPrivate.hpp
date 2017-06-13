/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENEEXTENSIONS_MODELCOMPONENTPRIVATE
#define GRAPEVINE_SCENEEXTENSIONS_MODELCOMPONENTPRIVATE

#include "Grapevine/SceneExtensions/ModelComponent.hpp"
#include "ModelComponentPrivate.hpp"
#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Scene/AbstractComponentPrivateTemplate.hpp>
#include <vector>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            class ModelComponentPrivate: public AbstractComponentPrivateTemplate<ModelComponentPrivate>
            {
            public:
                CLASS_UNCOPYABLE(ModelComponentPrivate)

                ModelComponentPrivate();
                ~ModelComponentPrivate();
                void addMeshMaterialPair(MeshAsset& mesh, MaterialAsset& material);
                std::vector<ModelComponent::MeshMaterialPair>& meshMaterialPairs();

            private:
                std::vector<ModelComponent::MeshMaterialPair> meshMaterialPairs_;
                float opacity_;
            };
        }
    }
}

#endif
