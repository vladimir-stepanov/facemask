/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "ImageResource.hpp"
#include "Grapevine/Image/Bitmap.hpp"
#include "Grapevine/Image/ImagePng.hpp"
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Core/Exception.hpp>

namespace Grapevine
{
    ImageResource::ImageResource(std::string const& filename):
        Resource(filename),
        bitmap_()
    {
        ImagePng png;

        png.gotData(path());

        unsigned int dataLength = png.width() * png.height() * png.pixelBytes();
        char* data = new char[dataLength];

        for (unsigned int i = 0; i < png.height(); i++)
        {
            memcpy(&data[png.pixelBytes() * png.width() * i], png.row(png.height() - i - 1), png.width() * png.pixelBytes());
        }

        bitmap_ = Bitmap(data, png.width(), png.height(), png.pixelBytes());
    }

    ImageResource::ImageResource(Bitmap& bitmap):
        Resource(""),
        bitmap_(bitmap)
    {
    }

    ImageResource::ImageResource(ImageResource const& r, unsigned int x, unsigned int y, unsigned int w, unsigned int h):
        Resource(""),
        bitmap_(r.bitmap_)
    {
        /*
         * // TODO check x and y

        // Check that we are not copying mem over the "right" of the image buffer
        if ((x + w) > r.bitmap_.width())
        {
            w = r.bitmap_.width() - x;
        }

        // Check that we are not copying mem over the "bottom" of the image buffer
        if ((y + h) > r.bitmap_.height())
        {
            h = r.bitmap_.height() - y;
        }

        // Copy mem
        Bitmap::Format::Enum format = r.bitmap_.format();
        if(Bitmap::Format::RGBA_8888 == format)
        {
            char* target = new char[w * h * 4];
            unsigned int targetIndex = 0;
            unsigned int sourceIndex = 0;
            unsigned int sourceImageRowBytes = r.bitmap_.width() * 4;
            unsigned int sourceImageColumnStart = x * 4;
            unsigned int targetImageRowBytes = w * 4;
            char* source = r.bitmap_.buffer()->data();
            for (unsigned int row = 0; row < h; row++)
            {
                sourceIndex = (row * sourceImageRowBytes) + sourceImageColumnStart;
                memcpy(target + targetIndex, source + sourceIndex, targetImageRowBytes);
                targetIndex += targetImageRowBytes;
            }

            bitmap_ = Bitmap(target, w, h, format);
        }
        else
        {
            LOGE("format not supported!");
        }*/
    }

    ImageResource::~ImageResource()
    {
    }

    char* ImageResource::pixels()
    {
        return bitmap_.buffer()->data();
    }

    unsigned int ImageResource::width()
    {
        return bitmap_.width();
    }

    unsigned int ImageResource::height()
    {
        return bitmap_.height();
    }

    Bitmap::Format::Enum ImageResource::format() const
    {
        return bitmap_.format();
    }
    /*unsigned int ImageResource::bytesPerPixel()
    {
        return bitmap_.bytesPerPixel();
    }*/
}
