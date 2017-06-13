/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_IMAGE_IMAGERESOURCE
#define GRAPEVINE_IMAGE_IMAGERESOURCE

#include "Grapevine/Resource/Resource.hpp"
#include <Grapevine/Image/Bitmap.hpp>
#include <Grapevine/Core/Class.hpp>

namespace Grapevine
{
    class ImageResource: public Resource
    {
    public:
        CLASS_UNCOPYABLE(ImageResource);

        ImageResource(std::string const& filename);
        ImageResource(Bitmap& filename);

        /**
         * Copy part of the image.
         *
         * @param   r   source
         * @param   x   source x
         * @param   y   source y
         * @param   w   target w
         * @param   h   target h
         */
        ImageResource(ImageResource const& r, unsigned int x, unsigned int y, unsigned int w, unsigned int h);
        ~ImageResource();
        char* pixels();
        unsigned int width();
        unsigned int height();
        Bitmap::Format::Enum format() const;
        //unsigned int bytesPerPixel();

    private:
        Bitmap bitmap_;
    };
}

#endif

