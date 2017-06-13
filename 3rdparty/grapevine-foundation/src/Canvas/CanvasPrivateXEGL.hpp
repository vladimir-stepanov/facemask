/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CANVAS_CANVASPRIVATEXEGL
#define GRAPEVINE_CANVAS_CANVASPRIVATEXEGL

#include "CanvasPrivate.hpp"
#include <Grapevine/GPU/GL.hpp>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <EGL/egl.h>

namespace Grapevine
{
    class CanvasPrivateXEGL: public CanvasPrivate
    {
    public:
        CanvasPrivateXEGL(unsigned int width, unsigned int height);
        virtual ~CanvasPrivateXEGL();
        void clear();
        void resize(unsigned int width, unsigned int height);
        void update();
        Display* xDisplay() const;
        void setClearColor(Vector4 const& color);

    private:
        bool supportsGL2();
        bool setContext();
        void initGLExtensions();
        bool renderToMe();

        // EGL
        bool EGLDisplayCheck();
        bool EGLConfigCheck();
        bool EGLSurfaceCheck();
        EGLDisplay EGLDisplay_;
        EGLSurface EGLSurface_;
        EGLConfig EGLConfig_;
        EGLContext EGLContext_;

        // X
        bool ensureXWindow();
        XVisualInfo *xVisualInfo();
        Window xWindow_;
        Display* xDisplay_;
    };
}

#endif
