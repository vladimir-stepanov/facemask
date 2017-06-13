/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "CanvasPrivateXEGL.hpp"
#include <Grapevine/GPU/GL.hpp>
#include <Grapevine/Core/Exception.hpp>
#include <Grapevine/Core/Util.hpp>
#include <Grapevine/Core/Log.hpp>
#include <fstream>
#include <sstream>

namespace Grapevine
{
    CanvasPrivateXEGL::CanvasPrivateXEGL(unsigned int width, unsigned int height):
        CanvasPrivate(width, height),
        EGLDisplay_(EGL_NO_DISPLAY),
        EGLSurface_(EGL_NO_SURFACE),
        EGLConfig_(0),
        EGLContext_(EGL_NO_CONTEXT),
        xWindow_(0),
        xDisplay_(0)
    {
        Log::I("CanvasPrivateXEGL constructor, start");
        xDisplay_ = XOpenDisplay(NULL);
        if (!xDisplay_)
        {
            throw Exception("Could not open XDisplay!");
        }
        resize(width_, height_);

        if (!xWindow_)
        {
            throw Exception("No X Window!");
        }

        if (!renderToMe())
        {
            throw Exception("Could not select canvas to be the context!");
        }

        if (!supportsGL2())
        {
            LOGE("OpenGL(ES) version >= 2.0 needed! Version is: %s", glGetString(GL_VERSION));
            throw Exception("CanvasPrivateXEGL(), OpenGL(ES) version >= 2.0 needed.");
        }

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        //glEnable(GL_CULL_FACE);
        //glCullFace(GL_BACK);
        clear();
        Log::I("CanvasPrivateXEGL constructor, end");
    }

    CanvasPrivateXEGL::~CanvasPrivateXEGL()
    {
        eglMakeCurrent(EGLDisplay_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(EGLDisplay_, EGLContext_);
        eglDestroySurface(EGLDisplay_, EGLContext_);
        eglTerminate(EGLDisplay_);
    }

    void CanvasPrivateXEGL::clear()
    {
        glClearColor(clearColor_.x(), clearColor_.y(), clearColor_.z(), clearColor_.w());
        glClearDepthf(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void CanvasPrivateXEGL::resize(unsigned int width, unsigned int height)
    {
        width_ = width;
        height_ = height;

        // If window and size changed-> destroy
        if (xWindow_)
        {
            if (width_ != width || height_ != height)
            {
                XDestroyWindow(xDisplay_, xWindow_);
                xWindow_ = 0;
            }
            else
            {
                return;
            }
        }

        // Ensure window
        width_ = width;
        height_ = height;
        if (!ensureXWindow())
        {
            throw Exception("Couldn't create X Window!");
        }

        XMapWindow(xDisplay_, xWindow_);

        // Update GL
        glViewport(0, 0, width_, height_);
    }

    void CanvasPrivateXEGL::update()
    {
        eglSwapBuffers(EGLDisplay_, EGLSurface_);
    }

    Display* CanvasPrivateXEGL::xDisplay() const
    {
        return xDisplay_;
    }

    bool CanvasPrivateXEGL::setContext()
    {
        bool r = false;

        initGLExtensions();
        r = true;

        return r;
    }

    bool CanvasPrivateXEGL::supportsGL2()
    {
        bool r = false;

        const GLubyte* tmpVersion = glGetString(GL_VERSION);
        if (tmpVersion == NULL)
        {
            Log::E("CanvasXEGL supportsGL2 glGetString == NULL (?)");
            return true;
        }
        std::string tmp((char const*)tmpVersion);

        int glMajor(0);
        size_t pointPosition(tmp.find('.'));

        if (pointPosition != std::string::npos)
        {
            pointPosition--;
            size_t startPosition(tmp.rfind(' ', pointPosition));
            if (startPosition == std::string::npos)
            {
                startPosition = 0;
            }
            else
            {
                startPosition++;
            }
            glMajor = Util::fromString<int>(tmp.substr(startPosition,
                pointPosition - startPosition + 1));
        }
        r = glMajor >= 2;

        return r;
    }

    void CanvasPrivateXEGL::initGLExtensions()
    {
        // Parse extension string to check if function pointers exist
        std::string extensionsString;
        const char* extensions = reinterpret_cast<const char*>(
            glGetString(GL_EXTENSIONS));

        if (extensions)
        {
            extensionsString = extensions;
        }

        if (extensionsString.find("GL_OES_mapbuffer") != std::string::npos)
        {
            GLExtensions::MapBuffer = reinterpret_cast<PFNGLMAPBUFFEROESPROC>(
                eglGetProcAddress("glMapBufferOES"));
            GLExtensions::UnmapBuffer = reinterpret_cast<PFNGLUNMAPBUFFEROESPROC>(
                eglGetProcAddress("glUnmapBufferOES"));
        }
    }

    bool CanvasPrivateXEGL::renderToMe()
    {
        if (!EGLSurfaceCheck())
        {
            Log::E("Failed EGLSurfaceCheck()");
            return false;
        }

        if (EGLContext_ == eglGetCurrentContext())
        {
            // Current context is the same
            return true;
        }

        if (!eglMakeCurrent(EGLDisplay_, EGLSurface_, EGLSurface_, EGLContext_))
        {
            Log::E("eglMakeCurrent failed, error %d", eglGetError());
            return false;
        }

        if (!eglSwapInterval(EGLDisplay_, 0))
        {
            Log::E("eglSwapInterval failed! Continuing...");
        }

        initGLExtensions();

        return true;
    }

    bool CanvasPrivateXEGL::EGLDisplayCheck()
    {
        if (EGLDisplay_)
        {
            return true;
        }

        EGLDisplay_ = eglGetDisplay((EGLNativeDisplayType) xDisplay_);
        if (!EGLDisplay_)
        {
            Log::E("eglGetDisplay() failed: %d", eglGetError());
            return false;
        }

        if (!eglInitialize(EGLDisplay_, NULL, NULL))
        {
            Log::E("eglInitialize() failed: %d", eglGetError());
            EGLDisplay_ = 0;
            return false;
        }

        return true;
    }

    bool CanvasPrivateXEGL::EGLConfigCheck()
    {
        static const EGLint attributes[] = {
            EGL_RED_SIZE, 5,
            EGL_GREEN_SIZE, 6,
            EGL_BLUE_SIZE, 5,
            EGL_ALPHA_SIZE, 0,
            EGL_DEPTH_SIZE, 0,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_NONE
        };
        EGLint num_configs;
        EGLint vid;

        if (EGLConfig_)
        {
            return true;
        }

        if (!EGLDisplayCheck())
        {
            return false;
        }

        if (!eglChooseConfig(EGLDisplay_, attributes, &EGLConfig_, 1, &num_configs))
        {
            Log::E("eglChooseConfig() failed with error: %d", eglGetError());
            return false;
        }

        if (!eglGetConfigAttrib(EGLDisplay_, EGLConfig_, EGL_NATIVE_VISUAL_ID, &vid))
        {
            Log::E("eglGetConfigAttrib() failed with error: %d", eglGetError());
            return false;
        }

        // Debug
        {
            int red;
            int green;
            int blue;
            int alpha;
            int depth;
            int buf;
            int configId;
            int nativeId;
            int sampleBuffers;
            int samples;
            eglGetConfigAttrib(EGLDisplay_, EGLConfig_, EGL_RED_SIZE, &red);
            eglGetConfigAttrib(EGLDisplay_, EGLConfig_, EGL_GREEN_SIZE, &green);
            eglGetConfigAttrib(EGLDisplay_, EGLConfig_, EGL_BLUE_SIZE, &blue);
            eglGetConfigAttrib(EGLDisplay_, EGLConfig_, EGL_ALPHA_SIZE, &alpha);
            eglGetConfigAttrib(EGLDisplay_, EGLConfig_, EGL_DEPTH_SIZE, &depth);
            eglGetConfigAttrib(EGLDisplay_, EGLConfig_, EGL_BUFFER_SIZE, &buf);
            eglGetConfigAttrib(EGLDisplay_, EGLConfig_, EGL_CONFIG_ID, &configId);
            eglGetConfigAttrib(EGLDisplay_, EGLConfig_, EGL_NATIVE_VISUAL_ID, &nativeId);
            eglGetConfigAttrib(EGLDisplay_, EGLConfig_, EGL_SAMPLE_BUFFERS, &sampleBuffers);
            eglGetConfigAttrib(EGLDisplay_, EGLConfig_, EGL_SAMPLES, &samples);
            Log::I("EGL Config Attibutes:\n"
                "    EGL_RED_SIZE:    %d\n"
                "    EGL_GREEN_SIZE:  %d\n"
                "    EGL_BLUE_SIZE:   %d\n"
                "    EGL_ALPHA_SIZE:  %d\n"
                "    EGL_DEPTH_SIZE:  %d\n"
                "    EGL_BUFFER_SIZE: %d\n"
                "    EGL_CONFIG_ID:   %d\n"
                "    EGL_NATIVE_VISUAL_ID: %d\n"
                "    EGL_SAMPLE_BUFFERS: %d\n"
                "    EGL_SAMPLES:     %d",
                red, green, blue, alpha, depth, buf, configId, nativeId, sampleBuffers, samples);
        }

        return true;
    }

    bool CanvasPrivateXEGL::EGLSurfaceCheck()
    {
        static const EGLint contextAttributes[] =
        {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
        };

        if (EGLSurface_)
        {
            return true;
        }

        if (!EGLDisplayCheck())
        {
            return false;
        }

        eglBindAPI(EGL_OPENGL_ES_API);

        EGLContext_ = eglCreateContext(EGLDisplay_, EGLConfig_, EGL_NO_CONTEXT,
            contextAttributes);
        if (!EGLContext_)
        {
            Log::E("eglCreateContext() failed with error: %d", eglGetError());
            return false;
        }

        EGLSurface_ = eglCreateWindowSurface(EGLDisplay_, EGLConfig_,
            (EGLNativeWindowType) xWindow_, NULL);
        if (!EGLSurface_)
        {
            Log::E("eglCreateWindowSurface failed with error: %d", eglGetError());
            return false;
        }

        return true;
    }

    bool CanvasPrivateXEGL::ensureXWindow()
    {
        if (xWindow_)
        {
            return true;
        }

        if (!xDisplay_)
        {
            Log::E("X11 Display has not been initialized!");
            return false;
        }

        XVisualInfo* visualInfo = xVisualInfo();
        if (!visualInfo)
        {
            Log::E("Could not get a valid XVisualInfo!");
            return false;
        }

        Log::I("Creating XWindow W:%d H:%d VisualID:0x%x", width_, height_,
            visualInfo->visualid);

        // Create X Window
        XSetWindowAttributes attributes;
        unsigned long mask;
        Window root = RootWindow(xDisplay_, DefaultScreen(xDisplay_));
        attributes.background_pixel = 0;
        attributes.border_pixel = 0;
        attributes.colormap = XCreateColormap(xDisplay_, root, visualInfo->visual,
            AllocNone);
        attributes.event_mask = KeyPressMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask;
        mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;
        xWindow_ = XCreateWindow(xDisplay_, root, 0, 0, width_, height_,
            0, visualInfo->depth, InputOutput, visualInfo->visual, mask,
            &attributes);

        XFree(visualInfo);
        if (!xWindow_)
        {
            Log::E("XCreateWindow() failed!");
            return false;
        }

        // Size hints
        static char const* name("grapevine");
        XSizeHints sizehints;
        sizehints.min_width  = width_;
        sizehints.min_height = height_;
        sizehints.max_width  = width_;
        sizehints.max_height = height_;
        sizehints.flags = PMaxSize | PMinSize;
        XSetNormalHints(xDisplay_, xWindow_, &sizehints);
        XSetStandardProperties(xDisplay_, xWindow_, name, name, None, NULL, 0,
            &sizehints);

        // Window Delete event from window manager
        Atom wmDelete = XInternAtom(xDisplay_, "WM_DELETE_WINDOW", True);
        XSetWMProtocols(xDisplay_, xWindow_, &wmDelete, 1);

        return true;
    }

    XVisualInfo* CanvasPrivateXEGL::xVisualInfo()
    {
        XVisualInfo infoTemplate;
        XVisualInfo* info;
        int num;
        EGLint vid;

        if (!EGLConfigCheck())
        {
            Log::E("Failed EGLConfigCheck()");
            return 0;
        }

        if (!eglGetConfigAttrib(EGLDisplay_, EGLConfig_, EGL_NATIVE_VISUAL_ID, &vid))
        {
            Log::E("Failed eglGetConfigAttrib(): %d", eglGetError());
            return 0;
        }

        // The X window visual must match the EGL config
        infoTemplate.visualid = vid;
        info = XGetVisualInfo(xDisplay_, VisualIDMask, &infoTemplate, &num);

        if (!info)
        {
            Log::E("Couldn't get X visual");
            return 0;
        }

        return info;
    }

    void CanvasPrivateXEGL::setClearColor(Vector4 const& color)
    {
        clearColor_ = color;
        glClearColor(clearColor_.x(), clearColor_.y(), clearColor_.z(), clearColor_.w());
    }
}
