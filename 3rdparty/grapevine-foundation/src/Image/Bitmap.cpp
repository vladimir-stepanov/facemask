/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <Grapevine/Image/Bitmap.hpp>
#include <png.h>

namespace Grapevine
{
    Bitmap::Bitmap(char* data, unsigned int width, unsigned int height, Bitmap::Format::Enum format, bool manageDataLifetime):
        Referenced(),
        buffer_(NULL),
        width_(width),
        height_(height),
        format_(format)
    {
        unsigned int sizeInBytes = size(format_, width_, height_);

        if(!sizeInBytes)
        {
            LOGE("Format is not supported!");
        }

        buffer_ = new Buffer(data, sizeInBytes, manageDataLifetime);
        buffer_->increment();
    }

    /** Bitmap class handles a rectangular buffer
     *
     * @data: pointer to block of data.
     * @width: width of the bitmap
     * @height: height of the bitmap
     * @bytesPerPixel: how many bytes there are per pixel,
     *      @note: format is forced then like next:
     *      1: LUMINANCE_8, 2: RGB_565, 3: RGB888, 4: RGBA8888.
     *      Other formats are not supported by this constructor!
     * @manageDataLifetime: true if data is owned and freed by the
     *      Bitmap. If false manager etc. existing the lifetime of
     *      this Bitmap must take care of the freeing.
     */
    Bitmap::Bitmap(char* data, unsigned int width, unsigned int height, unsigned int bytesPerPixel, bool manageDataLifetime):
        Referenced(),
        buffer_(NULL),
        width_(width),
        height_(height),
        format_(Bitmap::Format::RGBA_8888)
    {
        if(bytesPerPixel == 1)
        {
            format_ = Bitmap::Format::LUMINANCE_8;
        }
        else if(bytesPerPixel == 2)
        {
            format_ = Bitmap::Format::RGB_565;
        }
        else if(bytesPerPixel == 3)
        {
            format_ = Bitmap::Format::RGB_888;
        }
        else if(bytesPerPixel == 4)
        {
            format_ = Bitmap::Format::RGBA_8888;
        }
        else
        {
            LOGE("Unsupported bytesPerPixel value (%d), using RGBA_8888!", bytesPerPixel);
            format_ = Bitmap::Format::RGBA_8888;
        }

        buffer_ = new Buffer(data, width * height * bytesPerPixel, manageDataLifetime);
        buffer_->increment();
    }

    /** Bitmap class handles a rectangular buffer
     *
     * @dataBuffer: pointer to block of data.
     * @width: width of the bitmap
     * @height: height of the bitmap.
     * @format: format in buffer.
     */
    Bitmap::Bitmap(Buffer* dataBuffer, unsigned int width, unsigned int height, Bitmap::Format::Enum format):
        Referenced(),
        buffer_(dataBuffer),
        width_(width),
        height_(height),
        format_(format)
    {
        if(buffer_ != NULL)
        {
            buffer_->increment();
        }
    }

    Bitmap::Bitmap():
        Referenced(),
        buffer_(NULL),
        width_(0),
        height_(0),
        format_(Bitmap::Format::UNDEFINED)
    {
    }

    Bitmap::Bitmap(Bitmap const& r):
        Referenced(),
        buffer_(r.buffer_),
        width_(r.width_),
        height_(r.height_),
        format_(r.format_)
    {
        if(this == &r)
        {
            throw Exception("Bitmap constructed from itself!");
        }
        buffer_->increment();
    }

    Bitmap& Bitmap::operator=(Bitmap const& r)
    {
        if (this != &r)
        {
            // Old buffer
            if(buffer_ != NULL)
            {
                buffer_->decrement();
                buffer_ = NULL;
            }

            // New buffer
            buffer_ = r.buffer_;
            buffer_->increment();
            width_ = r.width_;
            height_ = r.height_;
            format_ = r.format_;
        }
        return *this;
    }

    Bitmap::~Bitmap()
    {
        if (buffer_ != NULL)
        {
            buffer_->decrement();
        }
    }

    Buffer* Bitmap::buffer() const
    {
        return buffer_;
    }

    unsigned int Bitmap::width() const
    {
        return width_;
    }

    unsigned int Bitmap::height() const
    {
        return height_;
    }

    Bitmap::Format::Enum Bitmap::format() const
    {
        return format_;
    }
}
