/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <Grapevine/Resource/ResourceManagerPrivate.hpp>
#include "Grapevine/Material/CubemapAsset.hpp"
#include "Grapevine/Material/CubemapResource.hpp"

namespace Grapevine
{
    static inline ReferencedPointer<CubemapResource> get(std::string const& filename, CubemapAsset::Type::Enum type)
    {
        std::string typeString = type == CubemapAsset::Type::HorizontalCross ? "HorizontalCross" : "UnknownType";
        std::string id = std::string("cubemap") + typeString + std::string(filename);
        CubemapResource* tmp = dynamic_cast<CubemapResource*>(ResourceManagerPrivate::resource(id));
        if (tmp == NULL)
        {
            tmp = new CubemapResource(filename, type);
            ResourceManagerPrivate::setResource(id, tmp);
        }
        return ReferencedPointer<CubemapResource>(tmp);
    }

    CubemapAsset::CubemapAsset(char const* filename, Type::Enum type):
        resource_(NULL)
    {
        resource_ = get(std::string(filename), type);
    }

    CubemapAsset::CubemapAsset(std::string const& filename, Type::Enum type):
        resource_(NULL)
    {
        resource_ = get(filename, type);
    }

    CubemapAsset::CubemapAsset(CubemapAsset const& r):
        resource_(r.resource_)
    {
    }

    CubemapAsset::CubemapAsset(Bitmap& bitmap, Type::Enum type):
        resource_(NULL)
    {
        // TODO: hash or similar to use ResourceManager also for preloaded bitmap data?
        resource_ = ReferencedPointer<CubemapResource>(new CubemapResource(bitmap, type));
    }

    CubemapAsset::~CubemapAsset()
    {
    }

    CubemapAsset& CubemapAsset::operator=(CubemapAsset const& r)
    {
        if (this != &r)
        {
            resource_ = r.resource_;
        }
        return *this;
    }

    CubemapResource* CubemapAsset::resource()
    {
        return resource_.pointer();
    }
}
