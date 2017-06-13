/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "CanvasPrivate.hpp"

namespace Grapevine
{
    CanvasPrivate::CanvasPrivate(unsigned int width, unsigned int height):
        width_(width),
        height_(height),
        clearColor_(Vector4(0.0f, 0.0f, 0.0f, 1.0f))
    {
    }

    unsigned int CanvasPrivate::width() const
    {
        return width_;
    }

    unsigned int CanvasPrivate::height() const
    {
        return height_;
    }

    void CanvasPrivate::setClearColor(Vector4 const& color)
    {
        clearColor_ = color;
    }
}
