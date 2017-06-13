/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATERIAL_FRAMEBUFFERASSET
#define GRAPEVINE_MATERIAL_FRAMEBUFFERASSET

#include "TextureAsset.hpp"

namespace Grapevine
{
    /**
     * @class FramebufferAsset
     *
     * FramebufferAsset is a class to be used for render target textures.
     */
    class FramebufferAsset : public TextureAsset
    {
    public:
        FramebufferAsset() {};
        FramebufferAsset(uint32_t width, uint32_t height);
        void bind();
        void unbind();
    };
}

#endif
