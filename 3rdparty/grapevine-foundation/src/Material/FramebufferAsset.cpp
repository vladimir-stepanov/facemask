/**
 * Copyright (C) 2013-2016 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <Grapevine/Material/FramebufferAsset.hpp>
#include <Grapevine/Material/TextureResource.hpp>

namespace Grapevine
{
    FramebufferAsset::FramebufferAsset(uint32_t width, uint32_t height)
    : TextureAsset(Bitmap::Format::RGBA_8888, width, height, 0, nullptr, false, false)
    {

    }

    void FramebufferAsset::bind()
    {
        resource_->bindAsFramebuffer();
    }

    void FramebufferAsset::unbind()
    {
        resource_->unbindFramebuffer();
    }

}
