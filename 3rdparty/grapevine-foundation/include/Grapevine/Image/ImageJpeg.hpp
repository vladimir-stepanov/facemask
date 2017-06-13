/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_IMAGE_IMAGEJPEG
#define GRAPEVINE_IMAGE_IMAGEJPEG

#include <stdio.h>
extern "C" {
#include <jpeglib.h>
}
#include <string>
#include <setjmp.h>

namespace Grapevine
{
    class ImageJpeg
    {
    public:
        ImageJpeg();
        ~ImageJpeg();
        void gotData(std::string const& filename);
        unsigned int width() const;
        unsigned int height() const;
        unsigned int pixelBytes() const;
        unsigned char* row(unsigned int idx);
        unsigned char* imageData();


    private:
        struct error_mgr {
          struct jpeg_error_mgr pub;
          jmp_buf setjmp_buffer;
        };
        typedef struct error_mgr * error_ptr;

        void readJPEGFile (char * filename);
        unsigned char* imageData_;
        int width_;
        int height_;
        int numChannels_;
    };
}

#endif
