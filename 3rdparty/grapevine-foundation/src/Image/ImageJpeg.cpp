/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Image/ImageJpeg.hpp"
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Core/Util.hpp>
#include <Grapevine/Core/Exception.hpp>
#include <Grapevine/Resource/File.hpp>
extern "C" {
#include <jpeglib.h>
}
#include <memory>
#include <stdlib.h>
#include <string.h>

namespace Grapevine
{
    ImageJpeg::ImageJpeg():
        width_(0),
        height_(0),
        numChannels_(0),
        imageData_(nullptr)
    {
    }

    ImageJpeg::~ImageJpeg()
    {
        if(imageData_)
        {
            free(imageData_);
        }
    }

    void ImageJpeg::gotData(std::string const& filename)
    {
        LOGD("Reading JPEG file %s", filename.c_str());
        readJPEGFile((char*)filename.c_str());
    }

    unsigned int ImageJpeg::width() const
    {
        return width_;
    }

    unsigned int ImageJpeg::height() const
    {
        return height_;
    }

    unsigned int ImageJpeg::pixelBytes() const
    {
        return numChannels_;
    }

    unsigned char* ImageJpeg::row(unsigned int idx) {
        return &imageData_[idx];
    }

    unsigned char* ImageJpeg::imageData() {
        return imageData_;
    }

    void ImageJpeg::readJPEGFile (char *filename)
    {
        struct jpeg_decompress_struct cinfo;
        FILE * infile;
        JSAMPARRAY buffer;
        int row_stride;
        long counter = 0;
        struct error_mgr jerr;

        LOGD("Opening JPEG");

        if ((infile = fopen(filename, "rb")) == NULL) {
            Log::E("Cannot open file %s\n", filename);
            throw Exception("Cannot open JPEG file");
        }

        cinfo.err = jpeg_std_error(&jerr.pub);
        //jerr.pub.error_exit = error_exit;
        if (setjmp(jerr.setjmp_buffer)) {
            jpeg_destroy_decompress(&cinfo);
            fclose(infile);

            Log::E("libjpeg error while reading file %s\n", filename);
            throw Exception("libjpeg error while reading file");
        }

        jpeg_create_decompress(&cinfo);

        jpeg_stdio_src(&cinfo, infile);

        (void) jpeg_read_header(&cinfo, TRUE);
        (void) jpeg_start_decompress(&cinfo);

        width_ = cinfo.output_width;
        height_ = cinfo.output_height;

        numChannels_ = cinfo.num_components;
        unsigned long dataSize = width_ * height_ * numChannels_;

        LOGD("Data size=%d", dataSize);

        imageData_ = (unsigned char *)malloc( dataSize );

        unsigned char* rowptr;

        while ( cinfo.output_scanline < height_ )
        {
            rowptr = imageData_ + ((cinfo.image_height-1-cinfo.output_scanline) * width_ * numChannels_);
            jpeg_read_scanlines(&cinfo, &rowptr, 1);
        }

        (void) jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);

        return;
    }
}

