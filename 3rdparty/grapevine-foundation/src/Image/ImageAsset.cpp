/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <Grapevine/Resource/ResourceManagerPrivate.hpp>
#include "Grapevine/Image/ImageAsset.hpp"
#include "ImageResource.hpp"

namespace Grapevine
{
    ImageAsset::ImageAsset(std::string const& filename):
        resource_(NULL)
    {
        LOGD("ImageAsset::ImageAsset filename %s", filename.c_str() );

        std::string id = "image" + filename;
        ImageResource* tmpResource = dynamic_cast<ImageResource*>(ResourceManagerPrivate::resource(id));
        if (tmpResource == NULL)
        {
            tmpResource = new ImageResource(filename);
            ResourceManagerPrivate::setResource(id, tmpResource);
        }
        resource_ = ReferencedPointer<ImageResource>(tmpResource);
    }

    ImageAsset::ImageAsset(ImageAsset const& r):
        resource_(r.resource_)
    {
    }

    ImageAsset::~ImageAsset()
    {
    }

    ImageAsset& ImageAsset::operator=(ImageAsset const& r)
    {
        if (this != &r)
        {
            resource_ = r.resource_;
        }
        return *this;
    }

    char* ImageAsset::pixels()
    {
        return resource_.pointer()->pixels();
    }

    unsigned int ImageAsset::width()
    {
        return resource_.pointer()->width();
    }

    unsigned int ImageAsset::height()
    {
        return resource_.pointer()->width();
    }

    /*unsigned int ImageAsset::bytesPerPixel()
    {
        return resource_.pointer()->bytesPerPixel();
    }*/
}
