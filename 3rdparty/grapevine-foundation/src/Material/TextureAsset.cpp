/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <Grapevine/Resource/ResourceManagerPrivate.hpp>
#include "Grapevine/Material/TextureAsset.hpp"
#include "Grapevine/Material/TextureResource.hpp"

namespace Grapevine
{
    TextureAsset::TextureAsset(char const* filename):
        resource_(NULL)
    {
        std::string id = "texture" + std::string(filename);
        resource_ = dynamic_cast<TextureResource*>(ResourceManagerPrivate::resource(id));
        if (resource_ == NULL)
        {
            resource_ = new TextureResource(filename);
            ResourceManagerPrivate::setResource(id, resource_);
        }
        resource_->increment();
    }

    TextureAsset::TextureAsset()
    :   resource_(NULL)
    {
    }

    TextureAsset::TextureAsset(TextureResource* resource)
    :   resource_(resource)
    {

    }

    TextureAsset::TextureAsset(std::string const& filename):
        resource_(NULL)
    {
        std::string id = "texture" + filename;
        resource_ = dynamic_cast<TextureResource*>(ResourceManagerPrivate::resource(id));
        if (resource_ == NULL)
        {
            resource_ = new TextureResource(filename);
            ResourceManagerPrivate::setResource(id, resource_);
        }
        resource_->increment();
    }

    TextureAsset::TextureAsset(Bitmap& bitmap, bool mipmaps):
        resource_(NULL)
    {
        resource_ = new TextureResource(bitmap, mipmaps);
        resource_->increment();
    }

    TextureAsset::TextureAsset(uint32_t EGLTextureId, bool mipmaps, bool managed, bool external, bool compressed):
        resource_(NULL)
    {
        resource_ = new TextureResource(EGLTextureId, mipmaps, managed, external, compressed);
        resource_->increment();
    }

    TextureAsset::TextureAsset(uint32_t internalFormat, uint32_t width, uint32_t height, uint32_t size, const uint8_t *data, bool compressed, bool mipmaps)
    {
        resource_ = new TextureResource(internalFormat, width, height, size, data, compressed, mipmaps);
        resource_->increment();
    }

    TextureAsset::TextureAsset(TextureAsset const& r):
        resource_(r.resource_)
    {
        if(resource_ != NULL)
        {
            resource_->increment();
        }
    }

    TextureAsset::~TextureAsset()
    {
        if(resource_ != NULL)
        {
            resource_->decrement();
        }
    }

    void TextureAsset::initializeGPUData()
    {
        resource_->id();
    }

    TextureAsset& TextureAsset::operator=(TextureAsset const& r)
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

    TextureResource* TextureAsset::resource()
    {
        return resource_;
    }
}
