/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_IMAGE_BITMAP
#define GRAPEVINE_IMAGE_BITMAP

#include <Grapevine/Core/Buffer.hpp>
#include <png.h>

namespace Grapevine
{
    /**
     * @class Bitmap
     *
     * Bitmap is a class which can hold rectangular pixel buffers
     */
    class Bitmap: public Referenced
    {
    public:
        // CLASS_COPYABLE

        struct Format
        {
            enum Enum
            {
                UNDEFINED = 0,
                LUMINANCE_8 = 1,
                RGB_888 = 2,
                RGBA_8888 = 3,
                RGB_565 = 4,
                YUV420SP_NV12 = 5,
                YUV420SP_NV21 = 6
            };
        };

        static unsigned int size(unsigned int format, int width, int height)
        {
            unsigned int size;

            switch(format)
            {
                case Bitmap::Format::LUMINANCE_8:
                {
                    size = width * height;
                    break;
                }
                case Bitmap::Format::RGB_888:
                {
                    size = width * height * 3;
                    break;
                }
                case Bitmap::Format::RGBA_8888:
                {
                    size = width * height * 4;
                    break;
                }
                case Bitmap::Format::RGB_565:
                {
                    size = width * height * 2;
                    break;
                }
                case Bitmap::Format::YUV420SP_NV12:
                case Bitmap::Format::YUV420SP_NV21:
                {
                    size = (width * height) + (2 * (((width + 1) / 2) * ((height + 1) / 2)));
                    break;
                }
                default:
                {
                    size = 0;
                }
            }

            LOGD("Bitmap size %i", size);

            return size;
        }

        /** Bitmap class handles a rectangular buffer
         *
         * @data: pointer to block of data.
         * @width: width of the bitmap
         * @height: height of the bitmap
         * @format: format of the bitmap
         * @manageDataLifetime: true if data is owned and freed by the
         *      Bitmap. If false manager etc. existing the lifetime of
         *      this Bitmap must take care of the freeing.
         */
        Bitmap(char* data, unsigned int width, unsigned int height, Format::Enum format, bool manageDataLifetime = true);


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
        Bitmap(char* data, unsigned int width, unsigned int height, unsigned int bytesPerPixel, bool manageDataLifetime = true);

        /** Bitmap class handles a rectangular buffer
         *
         * @dataBuffer: pointer to block of data.
         * @width: width of the bitmap
         * @height: height of the bitmap.
         * @format: format in buffer.
         */
        Bitmap(Buffer* dataBuffer, unsigned int width, unsigned int height, Format::Enum format = Format::RGBA_8888);

        Bitmap();

        Bitmap(Bitmap const& r);

        Bitmap& operator=(Bitmap const& r);

        ~Bitmap();

        Buffer* buffer() const;

        unsigned int width() const;

        unsigned int height() const;

        Format::Enum format() const;

    private:
        Buffer* buffer_;
        unsigned int width_;
        unsigned int height_;
        Format::Enum format_;
    };

}

#endif

