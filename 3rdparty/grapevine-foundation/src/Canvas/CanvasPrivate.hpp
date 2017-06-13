/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CANVAS_CANVASPRIVATE
#define GRAPEVINE_CANVAS_CANVASPRIVATE

#include <Grapevine/Math/Vector.hpp>

namespace Grapevine
{
    class CanvasPrivate
    {
    public:
        virtual ~CanvasPrivate()
        {
        };

        unsigned int width() const;
        unsigned int height() const;
        void setClearColor(Vector4 const& color);
        virtual void clear() = 0;
        virtual void resize(unsigned int width, unsigned int height) = 0;
        virtual void update() = 0;

    protected:
        CanvasPrivate(unsigned int width, unsigned int height);
        unsigned int width_;
        unsigned int height_;
        Vector4 clearColor_;
    };
}

#endif
