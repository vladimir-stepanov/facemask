/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Canvas/Canvas.hpp"
#include "src/Canvas/CanvasPrivateXEGL.hpp"

namespace Grapevine
{
    Canvas::Canvas(unsigned int width, unsigned int height):
        private_(new CanvasPrivateXEGL(width, height))
    {
    }

    Canvas::~Canvas()
    {
        delete private_;
    }

    unsigned int Canvas::width() const
    {
        return private_->width();
    }

    unsigned int Canvas::height() const
    {
        return private_->height();
    }

    void Canvas::clear()
    {
        private_->clear();
    }

    void Canvas::resize(unsigned int width, unsigned int height)
    {
        private_->resize(width, height);
    }

    void Canvas::update()
    {
        private_->update();
    }

    void Canvas::setClearColor(Vector4 const& color)
    {
        private_->setClearColor(color);
    }

    void Canvas::setResolution(const Vector2i&)
    {

    }

    Vector2i Canvas::resolution() const
    {
        return {width(), height()};
    }
}
