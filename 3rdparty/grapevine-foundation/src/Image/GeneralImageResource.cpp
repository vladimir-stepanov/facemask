/**
 * Copyright (C) 2013-2015 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "GeneralImageResource.hpp"
#include "Grapevine/Image/Bitmap.hpp"
#include "Grapevine/Image/ImagePng.hpp"
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Core/Exception.hpp>

namespace Grapevine
{
    GeneralImageResource::GeneralImageResource(uint32_t internalFormat,
        uint32_t width, uint32_t height, uint32_t size, const uint8_t *data, bool compressedFormat):
        Resource(""),
        data_(data),
        width_(width),
        height_(height),
        format_(internalFormat),
        size_(size),
        compressed_(compressedFormat)
    {
        LOGD("Compressed image resource. Format: 0x%08x, Size: %ix%i, Size: %i bytes", internalFormat, width, height, size);
    }

    GeneralImageResource::~GeneralImageResource()
    {
    }

    const uint8_t *GeneralImageResource::data() const
    {
        return data_;
    }

    uint32_t GeneralImageResource::width() const
    {
        return width_;
    }

    uint32_t GeneralImageResource::height() const
    {
        return height_;
    }

    uint32_t GeneralImageResource::format() const
    {
        return format_;
    }

    uint32_t GeneralImageResource::size() const
    {
        return size_;
    }

    bool GeneralImageResource::compressed() const
    {
        return compressed_;
    }
}
