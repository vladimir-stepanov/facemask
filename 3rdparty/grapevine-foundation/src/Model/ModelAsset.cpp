/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <Grapevine/Resource/ResourceManagerPrivate.hpp>
#include "Grapevine/Model/ModelAsset.hpp"
#include "Grapevine/Model/ModelResource.hpp"

namespace Grapevine
{
    ModelAsset::ModelAsset(std::string const& filename):
        resource_(NULL)
    {
        std::string id = "model" + filename;
        resource_ = dynamic_cast<ModelResource*>(ResourceManagerPrivate::resource(id));
        if (resource_ == NULL)
        {
            resource_ = new ModelResource(filename);
            ResourceManagerPrivate::setResource(id, resource_);
        }
        resource_->increment();
    }

    ModelAsset::ModelAsset(ModelAsset const& modelAsset):
        resource_(NULL)
    {
        resource_ = modelAsset.resource_;
        resource_->increment();
    }

    ModelAsset::~ModelAsset()
    {
        resource_->decrement();
    }

    ModelAsset& ModelAsset::operator=(ModelAsset const& r)
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

    ModelResource* ModelAsset::resource()
    {
        return resource_;
    }

    Box ModelAsset::boundingBox()
    {
        return Box(resource_->minBoundingVector(), resource_->maxBoundingVector());
    }
}
