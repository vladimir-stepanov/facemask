/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Text/FontAsset.hpp"
#include "FontResource.hpp"
#include "../Resource/ResourceManagerPrivate.hpp"

namespace Grapevine
{
    FontAsset::FontAsset(const FontAsset& r):
        resource_(r.resource_)
    {
        if (this == &r)
        {
            throw Exception("FontAsset constructed from itself!");
        }
        resource_->increment();
    }

    FontAsset::FontAsset(std::string const& filename, float scale)
    {
        std::string id = "font" + filename;
        resource_ = dynamic_cast<FontResource*>(ResourceManagerPrivate::resource(id));
        if (resource_ == NULL)
        {
            resource_ = new FontResource(filename);
            ResourceManagerPrivate::setResource(id, resource_);
        }
        resource_->increment();
    }

    FontAsset::~FontAsset()
    {
            resource_->decrement();
        }

    FontAsset& FontAsset::operator=(FontAsset const& r)
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

    FontResource* FontAsset::resource() const
    {
        return resource_;
    }

    std::string const& FontAsset::name() const
    {
        return resource_->path();
    }

    float FontAsset::getScale(float fontSize) const
    {
        return resource_->getScale(fontSize);
    }

    int FontAsset::getKerning(int left, int right) const
    {
        return resource_->getKerning(left, right);
    }

    int FontAsset::getAdvanceWidth(int code) const
    {
        return resource_->getAdvanceWidth(code);
    }

    bool FontAsset::operator==(const FontAsset& r) const
    {
        return *resource_ == *r.resource();
    }
}
