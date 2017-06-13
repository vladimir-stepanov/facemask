/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <Grapevine/Resource/ResourceManagerPrivate.hpp>
#include <Grapevine/Math/Box.hpp>
#include "Grapevine/Model/MeshAsset.hpp"
#include "Grapevine/Model/MeshResource.hpp"

namespace Grapevine
{
    MeshAsset::MeshAsset(std::string const& filename):
        resource_(NULL)
    {
        std::string id = "mesh" + filename;
        resource_ = dynamic_cast<MeshResource*>(ResourceManagerPrivate::resource(id));
        if (resource_ == NULL)
        {
            resource_ = new MeshResource(filename);
            ResourceManagerPrivate::setResource(id, resource_);
        }
        resource_->increment();
    }

    MeshAsset::MeshAsset(std::string const& name, class ModelData* modelData):
        resource_(NULL)
    {
        resource_ = new MeshResource(name, modelData);
        resource_->increment();
    }

    MeshAsset::MeshAsset(MeshAsset const& meshAsset):
        resource_(NULL)
    {
        resource_ = meshAsset.resource_;
        resource_->increment();
    }

    MeshAsset::MeshAsset(float width, float height, float textureUsageU, float textureUsageV, bool flipX, bool flipY):
        resource_(NULL)
    {
        resource_ = new MeshResource(width, height, textureUsageU, textureUsageV, flipX, flipY);
        resource_->increment();
    }

    MeshAsset::MeshAsset(float width, float height, Vector2 const& startPoint, Vector2 const& rectSize, bool flipX, bool flipY):
        resource_(NULL)
    {
        resource_ = new MeshResource(width, height, startPoint, rectSize, flipX, flipY);
        resource_->increment();
    }

    MeshAsset::MeshAsset(float width, float height, Vector2 const& positionOffset, Vector2 const& startPoint, Vector2 const& rectSize, bool flipX, bool flipY):
        resource_(NULL)
    {
        resource_ = new MeshResource(width, height, positionOffset, startPoint, rectSize, flipX, flipY);
        resource_->increment();
    }

    MeshAsset::MeshAsset(std::vector<Vector3> const& position, std::vector<Vector2> const& uv, std::vector<Vector3> const& normal, std::vector<uint16_t> const &indices)
    {
        resource_ = new MeshResource(position, uv, normal, indices);
        resource_->increment();
    }

    MeshAsset::MeshAsset(DynamicMesh const& dynamicMesh):
        resource_(NULL)
    {
        resource_ = new MeshResource(dynamicMesh);
        resource_->increment();
    }

    MeshAsset::MeshAsset(char* data, unsigned int byteSize):
        resource_(NULL)
    {
        resource_ = new MeshResource(data, byteSize);
        resource_->increment();
    }

    MeshAsset::~MeshAsset()
    {
        if(resource_ != NULL)
        {
            unsigned int tmp = resource_->decrement();
            if(tmp == 0)
            {
                LOGD("!!!!!!!!!!!!! deleted mesh resource!");
            }
        }
    }

    MeshAsset& MeshAsset::operator=(MeshAsset const& r)
    {
        if (this != &r)
        {
            if (resource_ != NULL)
            {
                resource_->decrement();
                resource_ = NULL;
            }

            if (r.resource_ != NULL)
            {
                resource_ = r.resource_;
                resource_->increment();
            }
        }
        return *this;
    }

    float MeshAsset::boundingSphereRadius() const
    {
        return resource_->boundingSphereRadius();
    }

    Box const& MeshAsset::boundingBox() const
    {
        return resource_->boundingBox();
    }

    void MeshAsset::initializeGPUData()
    {
        resource_->id();
    }

    MeshResource* MeshAsset::resource()
    {
        return resource_;
    }
}
