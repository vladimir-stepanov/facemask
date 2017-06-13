/**
 * Copyright (C) 2013-2015 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_IMAGE_GENERALIMAGERESOURCE
#define GRAPEVINE_IMAGE_GENERALIMAGERESOURCE

#include "Grapevine/Resource/Resource.hpp"
#include <Grapevine/Core/Class.hpp>

#include <vector>

namespace Grapevine
{
    class GeneralImageResource: public Resource
    {
    public:
        CLASS_UNCOPYABLE(GeneralImageResource);

        GeneralImageResource(uint32_t internalFormat, uint32_t width, uint32_t height, uint32_t size, const uint8_t *data, bool compressedFormat = false);
        ~GeneralImageResource();

        const uint8_t *data() const;
        uint32_t width() const;
        uint32_t height() const;
        uint32_t format() const;
        uint32_t size() const;
        bool compressed() const;

    private:
        const uint8_t *data_;
        uint32_t width_;
        uint32_t height_;
        uint32_t format_;
        uint32_t size_;
        bool compressed_;
    };
}

#endif

