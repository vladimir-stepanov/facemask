/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/SceneExtensions/ModelComponent.hpp"
#include "Grapevine/SceneExtensions/ModelComponentPrivate.hpp"
#include <Grapevine/Model/ModelResource.hpp>
#include <Grapevine/Material/TextureResource.hpp>
#include <Grapevine/Core/Exception.hpp>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            ModelComponent::ModelComponent():
                Component(new ModelComponentPrivate())
            {
                modelComponentPrivate_ = dynamic_cast<ModelComponentPrivate*>(private_.pointer());
            }

            ModelComponent::ModelComponent(ModelAsset modelAsset):
                Component(new ModelComponentPrivate())
            {
                modelComponentPrivate_ = dynamic_cast<ModelComponentPrivate*>(private_.pointer());

                ModelResource* resource = modelAsset.resource();
                if (resource->meshCount() == 0)
                {
                    throw Exception("Node::Node(), number of meshes is 0. Invalid model!");
                }
                else if (resource->meshCount() == 1)
                {
                    MeshAsset meshAsset = resource->mesh(0);
                    MaterialAsset materialAsset = resource->materialOfMeshIndex(0);
                    modelComponentPrivate_->addMeshMaterialPair(meshAsset, materialAsset);
                }
                else
                {
                    for (unsigned int i = 0; i < resource->meshCount(); i++)
                    {
                        MeshAsset meshAsset = resource->mesh(i);
                        MaterialAsset materialAsset = resource->materialOfMeshIndex(i);
                        modelComponentPrivate_->addMeshMaterialPair(meshAsset, materialAsset);
                    }
                }
            }

            ModelComponent::ModelComponent(MeshAsset meshAsset, MaterialAsset materialAsset):
                Component(new ModelComponentPrivate())
            {
                modelComponentPrivate_ = dynamic_cast<ModelComponentPrivate*>(private_.pointer());
                modelComponentPrivate_->addMeshMaterialPair(meshAsset, materialAsset);
            }

            ModelComponent::ModelComponent(ModelComponent const &r)
            : Component(r)
            {
                modelComponentPrivate_ = r.modelComponentPrivate_;
            }

            ModelComponent::~ModelComponent()
            {
            }

            ModelComponent &ModelComponent::operator=(ModelComponent const &r)
            {
                Component::operator=(r);
                modelComponentPrivate_ = r.modelComponentPrivate_;
                return *this;
            }

            void ModelComponent::addMeshMaterialPair(MeshAsset& mesh, MaterialAsset& material)
            {
                return modelComponentPrivate_->addMeshMaterialPair(mesh, material);
            }

            std::vector<ModelComponent::MeshMaterialPair>& ModelComponent::meshMaterialPairs()
            {
                return modelComponentPrivate_->meshMaterialPairs();
            }
        }
    }
}

