/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_IMAGE_IMAGEPNG
#define GRAPEVINE_IMAGE_IMAGEPNG

#include <png.h>
#include <string>

namespace Grapevine
{
    class ImagePng
    {
    public:
        ImagePng();
        ~ImagePng();
        void gotData(std::string const& filename);
        unsigned int width() const;
        unsigned int height() const;
        unsigned int pixelBytes() const;
        const unsigned char* row(unsigned int idx) const;
        const unsigned char** data() const;

    private:
        static void read(png_structp p, png_bytep data, png_size_t length);
        static void pngerror(png_structp png_ptr, png_const_charp error_message);
        png_structp png_;
        png_infop info_;
        png_bytepp rows_;
    };
}

#endif
