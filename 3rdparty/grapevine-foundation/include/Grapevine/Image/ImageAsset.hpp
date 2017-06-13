/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_IMAGE_IMAGEASSET
#define GRAPEVINE_IMAGE_IMAGEASSET

#include <string>
#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Core/ReferencedPointer.hpp>

namespace Grapevine
{
    /**
     * @class ImageAsset
     *
     * ImageAsset is a class which can hold an image file.
     *
     * @note Currently only .png format is supported.
     */
    class ImageAsset
    {
    public:
        CLASS_COPYABLE(ImageAsset)

        ImageAsset(std::string const& filename);
        ~ImageAsset();
        char* pixels();
        unsigned int width();
        unsigned int height();
        unsigned int bytesPerPixel();

    private:
        ReferencedPointer<class ImageResource> resource_;
    };
}

#endif

