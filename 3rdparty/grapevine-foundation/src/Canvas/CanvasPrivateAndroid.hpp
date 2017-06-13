/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CANVAS_CANVASPRIVATEANDROID
#define GRAPEVINE_CANVAS_CANVASPRIVATEANDROID

#include "CanvasPrivate.hpp"
#include <Grapevine/GPU/GL.hpp>
#include <EGL/egl.h>

namespace Grapevine
{
    class CanvasPrivateAndroid: public CanvasPrivate
    {
    public:
        CanvasPrivateAndroid(unsigned int width, unsigned int height):
            CanvasPrivate(width, height)
        {
            //glEnable(GL_CULL_FACE);
            //glCullFace(GL_BACK);
            //glClearDepthf(1.0f);
            //glDepthMask(true);
            //glDepthRangef(0.0f, 1.0f);
            //glDepthFunc(GL_GEQUAL);
            //glEnable(GL_DEPTH_TEST);
        }

        virtual ~CanvasPrivateAndroid()
        {
        }

        void clear()
        {
            glClearColor(clearColor_.r(), clearColor_.g(), clearColor_.b(), clearColor_.a());
            glClearDepthf(1.0f);
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        }

        void resize(unsigned int width, unsigned int height)
        {
            width_ = width;
            height_ = height;
        }

        void update()
        {
        }
    };
}

#endif
