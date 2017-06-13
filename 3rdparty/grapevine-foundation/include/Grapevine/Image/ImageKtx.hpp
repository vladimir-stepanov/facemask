/**
 * Copyright (C) 2013-2015 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_IMAGE_IMAGEKTX
#define GRAPEVINE_IMAGE_IMAGEKTX

#include <string>
#include <vector>

namespace Grapevine
{
    class ImageKtx
    {
    public:
        ImageKtx();
        ~ImageKtx();
        void loadHeader(std::string const& filename);
        void gotData();

        const uint8_t* data() const;
        uint32_t internalFormat() const;
        uint32_t width() const;
        uint32_t height() const;
        uint32_t size() const;

    private:
        // KTX file format header
        struct Header
        {
            uint8_t identifier_[12];
            uint32_t endianness_;
            uint32_t glType_;
            uint32_t glTypeSize_;
            uint32_t glFormat_;
            uint32_t glInternalFormat_;
            uint32_t glBaseInternalFormat_;
            uint32_t pixelWidth_;
            uint32_t pixelHeight_;
            uint32_t pixelDepth_;
            uint32_t numberOfArrayElements_;
            uint32_t numberOfFaces_;
            uint32_t numberOfMipmapLevels_;
            uint32_t bytesOfKeyValueData_;
        };

        FILE *ktxFile_;
        Header header_;
        std::vector<uint8_t> imageData_;
    };
}

#endif
