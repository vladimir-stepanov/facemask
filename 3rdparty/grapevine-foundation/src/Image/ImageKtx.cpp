/**
 * Copyright (C) 2013-2015 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Image/ImageKtx.hpp"
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Core/Util.hpp>
#include <Grapevine/Core/Exception.hpp>
#include <Grapevine/Resource/File.hpp>

namespace Grapevine
{
    ImageKtx::ImageKtx()
    {

    }

    ImageKtx::~ImageKtx()
    {

    }

    void ImageKtx::loadHeader(std::string const& filename)
    {
        static const uint8_t correctIdentifier[12] = {
            0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
        };

        if ((ktxFile_ = fopen(filename.c_str(), "rb")) == NULL) {
            LOGE("Cannot open file %s\n", filename.c_str());
            throw Exception("Cannot open KTX file");
        }

        fread(&header_, sizeof(Header), 1, ktxFile_);

        if(memcmp(correctIdentifier, header_.identifier_, sizeof(correctIdentifier))) {
            LOGE("Invalid KTX identifier!");
            fclose(ktxFile_);
            throw Exception("Cannot open KTX file");
        }

        if(header_.endianness_ != 0x04030201) {
            LOGE("Unsupported KTX endianness!");
            fclose(ktxFile_);
            throw Exception("Cannot open KTX file, unsupported endianness!");
        }

        if(header_.glType_) {
            LOGE("Uncompressed data found and only compressed KTX files supported!");
            fclose(ktxFile_);
            throw Exception("Cannot open KTX file, only compressed KTX files supported!");
        }

        fseek(ktxFile_, header_.bytesOfKeyValueData_, SEEK_CUR); // skip key-value data
    }

    void ImageKtx::gotData()
    {
        uint32_t imageSize = 0;
        fread(&imageSize, 4, 1, ktxFile_);

        LOGD("KTX image size: %i bytes", imageSize);

        imageData_.resize(imageSize);
        fread(&imageData_[0], 1, imageSize, ktxFile_);

        fclose(ktxFile_);
    }

    uint32_t ImageKtx::internalFormat() const
    {
        return header_.glInternalFormat_;
    }

    uint32_t ImageKtx::width() const
    {
        return header_.pixelWidth_;
    }

    uint32_t ImageKtx::height() const
    {
        return header_.pixelHeight_;
    }

    const uint8_t* ImageKtx::data() const
    {
        if(imageData_.empty())
        {
            return nullptr;
        }
        return &imageData_[0];
    }

    uint32_t ImageKtx::size() const
    {
        return imageData_.size();
    }
}
