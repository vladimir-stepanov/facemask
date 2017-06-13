/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENEEXTENSIONS_MODELCOMPONENT
#define GRAPEVINE_SCENEEXTENSIONS_MODELCOMPONENT

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Scene/Component.hpp>
#include <Grapevine/Model/ModelAsset.hpp>
#include <Grapevine/Model/MeshAsset.hpp>
#include <Grapevine/Material/MaterialAsset.hpp>
#include <vector>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            class ModelComponent: public Component
            {
            public:
                CLASS_COPYABLE(ModelComponent)

                struct MeshMaterialPair
                {
                    MeshMaterialPair(MeshAsset& meshAsset, MaterialAsset& materialAsset):
                        meshAsset(meshAsset),
                        materialAsset(materialAsset)
                    {
                    };

                    MeshAsset meshAsset;
                    MaterialAsset materialAsset;
                };

                ModelComponent();
                ModelComponent(ModelAsset modelAsset);

                /**
                 * Create ModelComponent
                 *
                 * @param [in]  modelAsset      ModelAsset to be used in the component
                 * @param [in]  materialAsset   MaterialAsset to be used in the component
                 */
                ModelComponent(MeshAsset modelAsset, MaterialAsset materialAsset);
                virtual ~ModelComponent();
                void addMeshMaterialPair(MeshAsset& mesh, MaterialAsset& material);
                std::vector<ModelComponent::MeshMaterialPair>& meshMaterialPairs();

            private:
                class ModelComponentPrivate* modelComponentPrivate_;
            };
        }
    }
}

#endif
