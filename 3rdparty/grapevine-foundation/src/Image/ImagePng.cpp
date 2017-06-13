/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Image/ImagePng.hpp"
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Core/Util.hpp>
#include <Grapevine/Core/Exception.hpp>
#include <Grapevine/Resource/File.hpp>
#include <png.h>
#include <memory>
#include <string.h>
#include <memory>

namespace Grapevine
{
    ImagePng::ImagePng(): png_(0), info_(0), rows_(0)
    {
    }

    ImagePng::~ImagePng()
    {
        if (png_)
        {
            png_destroy_read_struct(&png_, &info_, 0);
        }
    }

    void ImagePng::gotData(std::string const& filename)
    {
        static const int flags = PNG_TRANSFORM_STRIP_16 |
            PNG_TRANSFORM_GRAY_TO_RGB | PNG_TRANSFORM_PACKING |
            PNG_TRANSFORM_EXPAND;

        LOGD("Reading PNG file %s", filename.c_str());

        std::unique_ptr<File> file(new File(filename));

        /*const std::auto_ptr<std::istream> is_ptr(Util::getResource(filename));
        if (!(*is_ptr))
        {
            Log::E("Cannot open file %s!", filename.c_str());
            throw Exception("Cannot open file!");
        }*/

        // Create libpng structs
        png_ = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
        if (!png_)
        {
            throw Exception("Couldn't create libpng read struct");
        }

        info_ = png_create_info_struct(png_);
        if (!info_)
        {
            throw Exception("Couldn't create libpng info struct");
        }

        // Errors
        if (setjmp(png_jmpbuf(png_)))
        {
            png_destroy_read_struct(&png_, &info_, 0);
            png_ = nullptr;
            info_ = nullptr;

            Log::E("libpng error while reading file %s\n", filename.c_str());
            throw Exception("libpng error while reading file");
        }

        // png_set_error_fn(png_, NULL, pngerror, NULL);

        // Read the image header and data
        png_set_read_fn(png_, reinterpret_cast<void*>(file.get()), read);
        png_read_png(png_, info_, flags, 0);
        rows_ = png_get_rows(png_, info_);
    }

    unsigned int ImagePng::width() const
    {
        return png_get_image_width(png_, info_);
    }

    unsigned int ImagePng::height() const
    {
        return png_get_image_height(png_, info_);
    }

    unsigned int ImagePng::pixelBytes() const
    {
        int r = 4; // Default depth is 4
        if (png_get_color_type(png_, info_) == PNG_COLOR_TYPE_RGB)
        {
            r = 3;
        }
        return r;
    }

    const unsigned char* ImagePng::row(unsigned int idx) const
    {
        return rows_[idx];
    }

    const unsigned char** ImagePng::data() const
    {
        return (const unsigned char**)rows_;
    }

    void ImagePng::read(png_structp p, png_bytep data, png_size_t length)
    {
        File* file = reinterpret_cast<File*>(png_get_io_ptr(p));
        file->read(reinterpret_cast<char*>(data), (unsigned int)length);
    }

    void ImagePng::pngerror(png_structp png_ptr, png_const_charp error_message)
    {
        LOGE("libpng error occured: %s", error_message);
        // png_ptr->longjmp_fn(png_ptr->jmpbuf, 1);
    }

}
