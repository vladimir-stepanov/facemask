/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "Grapevine/Canvas/Canvas.hpp"
#include "Grapevine/GPU/GL.hpp"
#include <Grapevine/Core/Exception.hpp>
#include <Grapevine/Core/Util.hpp>
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Math/Vector.hpp>
#include <fstream>
#include <sstream>

#ifdef __NGOS__
#include "GonkDisplay.h"
#endif

#ifdef TSAIKKA
#include "hwcmanager/HWCManager.hpp"
#endif

using namespace Grapevine;

#ifdef __NGOS__
static mozilla::GonkDisplay* tGonkDisplay;
#endif

static EGLDisplay dpy;
static EGLContext ctx;
static EGLSurface srf;
static GLuint canvas_width, new_width;
static GLuint canvas_height, new_height;
static int canvas_resh, new_resh;
static int canvas_resv, new_resv;

static bool resize_needed = false;
static Vector4 clear_color;

static bool canvas_support_gl2()
{
    bool r = false;
    const GLubyte* tmp = glGetString(GL_VERSION);
    int major = 0;

    if (tmp == NULL) {
        Log::E("%s: glGetString == NULL (?)\n", __func__);
        return true;
    }

    std::string str((char const *) tmp);
    size_t cur(str.find('.'));

    if (cur != std::string::npos) {
        cur--;
        size_t start(str.rfind(' ', cur));

        if (cur == std::string::npos)
            start = 0;
        else
            start++;

        std::stringstream ss(str.substr(start, cur - start + 1));
        ss >> major;
    }
    r = major >= 2;
    return r;
}

static void canvas_init_extensions()
{
    /* Parse extension string to check if function pointers exist */
    std::string str;
    const char *ext = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));

    if (!ext) {
        Log::W("%s: no GL extensions found\n", __func__);
        return;
    }

    str = ext;
    if (str.find("GL_OES_mapbuffer") != std::string::npos) {
        GLExtensions::MapBuffer =
            reinterpret_cast<PFNGLMAPBUFFEROESPROC>(
            eglGetProcAddress("glMapBufferOES"));
        GLExtensions::UnmapBuffer =
            reinterpret_cast<PFNGLUNMAPBUFFEROESPROC>(
            eglGetProcAddress("glUnmapBufferOES"));
    }

    if (str.find("GL_EXT_disjoint_timer_query"))
    {
        GLExtensions::GetQueryObjectivEXT     = reinterpret_cast<decltype(GLExtensions::GetQueryObjectivEXT)>(eglGetProcAddress("glGetQueryObjectivEXT"));
        GLExtensions::GetQueryObjectuivEXT    = reinterpret_cast<decltype(GLExtensions::GetQueryObjectuivEXT)>(eglGetProcAddress("glGetQueryObjectuivEXT"));
        GLExtensions::GetQueryObjecti64vEXT   = reinterpret_cast<decltype(GLExtensions::GetQueryObjecti64vEXT)>(eglGetProcAddress("glGetQueryObjecti64vEXT"));
        GLExtensions::GetQueryObjectui64vEXT  = reinterpret_cast<decltype(GLExtensions::GetQueryObjectui64vEXT)>(eglGetProcAddress("glGetQueryObjectui64vEXT"));
        GLExtensions::GenQueriesEXT           = reinterpret_cast<decltype(GLExtensions::GenQueriesEXT)>(eglGetProcAddress("glGenQueriesEXT"));
        GLExtensions::DeleteQueriesEXT        = reinterpret_cast<decltype(GLExtensions::DeleteQueriesEXT)>(eglGetProcAddress("glDeleteQueriesEXT"));
        GLExtensions::IsQueryEXT              = reinterpret_cast<decltype(GLExtensions::IsQueryEXT)>(eglGetProcAddress("glIsQueryEXT"));
        GLExtensions::BeginQueryEXT           = reinterpret_cast<decltype(GLExtensions::BeginQueryEXT)>(eglGetProcAddress("glBeginQueryEXT"));
        GLExtensions::EndQueryEXT             = reinterpret_cast<decltype(GLExtensions::EndQueryEXT)>(eglGetProcAddress("glEndQueryEXT"));
        GLExtensions::GetQueryivEXT           = reinterpret_cast<decltype(GLExtensions::GetQueryivEXT)>(eglGetProcAddress("glGetQueryivEXT"));

        if(!GLExtensions::BeginQueryEXT)
        {
            LOGE("Found GL_EXT_disjoint_timer_query but didn't find the entry points");
        }
        else
        {
            GLExtensions::EXT_disjoint_timer_query = true;
        }
    }
}

#ifdef EGL_DEBUG
static void canvas_atrace(EGLDisplay dpy, EGLConfig cfg)
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

    eglGetConfigAttrib(dpy, cfg, EGL_RED_SIZE, &red);
    eglGetConfigAttrib(dpy, cfg, EGL_GREEN_SIZE, &green);
    eglGetConfigAttrib(dpy, cfg, EGL_BLUE_SIZE, &blue);
    eglGetConfigAttrib(dpy, cfg, EGL_ALPHA_SIZE, &alpha);
    eglGetConfigAttrib(dpy, cfg, EGL_DEPTH_SIZE, &depth);
    eglGetConfigAttrib(dpy, cfg, EGL_BUFFER_SIZE, &buf);
    eglGetConfigAttrib(dpy, cfg, EGL_CONFIG_ID, &configId);
    eglGetConfigAttrib(dpy, cfg, EGL_NATIVE_VISUAL_ID, &nativeId);
    eglGetConfigAttrib(dpy, cfg, EGL_SAMPLE_BUFFERS, &sampleBuffers);
    eglGetConfigAttrib(dpy, cfg, EGL_SAMPLES, &samples);
    Log::I("EGL Config Attibutes:\n"
           "    EGL_RED_SIZE:          %d\n"
           "    EGL_GREEN_SIZE:        %d\n"
           "    EGL_BLUE_SIZE:         %d\n"
           "    EGL_ALPHA_SIZE:        %d\n"
           "    EGL_DEPTH_SIZE:        %d\n"
           "    EGL_BUFFER_SIZE:       %d\n"
           "    EGL_CONFIG_ID:         %d\n"
           "    EGL_NATIVE_VISUAL_ID:  %d\n"
           "    EGL_SAMPLE_BUFFERS:    %d\n"
           "    EGL_SAMPLES:           %d",
           red, green, blue, alpha, depth, buf, configId, nativeId,
           sampleBuffers, samples);
}
#endif

Canvas::Canvas(unsigned int w, unsigned int h)
{
    static EGLConfig cfg;
    static const EGLint ctx_attrs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    static const EGLint attrs[] = {
        EGL_RED_SIZE, 1,
        EGL_GREEN_SIZE, 1,
        EGL_BLUE_SIZE, 1,
        EGL_ALPHA_SIZE, 1,
        EGL_DEPTH_SIZE, 1,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE,
    };
    GLint num;
    GLint vid;
    GLint major = 0;
    GLint minor = 0;

    new_resh = new_width = canvas_resh = canvas_width = w;
    new_resv = new_height = canvas_resv = canvas_height = h;

#ifdef TSAIKKA
    // Call this before EGL calls it also enforces HWComposer EGL backend
    EGLNativeWindowType win = HWCManager::getInstance()->createMainWindow(0, 0, w, h);
#endif
    dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (dpy == EGL_NO_DISPLAY) {
        Log::E("eglGetDisplay() failed, err=%d\n", eglGetError());
        throw new Exception("eglGetDisplay() failed");
    }

    if (!eglInitialize(dpy, &major, &minor)) {
        Log::E("eglInitialize() failed, err=%d\n", eglGetError());
        throw new Exception("eglInitialize() failed");
    }

    Log::I("EGL version %d.%d\n", major, minor);

    if (!eglChooseConfig(dpy, attrs, &cfg, 1, &num)) {
        Log::E("eglChooseConfig() failed, err=%d\n", eglGetError());
        throw new Exception("eglChooseConfig() failed");
    }

#ifdef __NGOS__
    tGonkDisplay = mozilla::GetGonkDisplay();
    if (!tGonkDisplay) {
        Log::E("GetGonkDisplay() failed\n");
        throw new Exception("GetGonkDisplay() failed");
    }

    srf = eglCreateWindowSurface(dpy, cfg, tGonkDisplay->GetNativeWindow(), NULL);
#else
#ifdef TSAIKKA
    srf = eglCreateWindowSurface(dpy, cfg, win, NULL);
#else
    srf = eglCreateWindowSurface(dpy, cfg, 0, NULL);
#endif
#endif

    if (srf == EGL_NO_SURFACE) {
        Log::E("eglCreateWindowSurface() failed, err=%d\n", eglGetError());
        throw new Exception("eglCreateWindowSurface() failed");
    }

    eglBindAPI(EGL_OPENGL_ES_API);

    ctx = eglCreateContext(dpy, cfg, EGL_NO_CONTEXT, ctx_attrs);

    if (ctx == EGL_NO_CONTEXT) {
        Log::E("eglCreateContext() failed, err=%d\n", eglGetError());
        throw new Exception("eglCreateContext() failed");
    }

    if (!eglMakeCurrent(dpy, srf, srf, ctx)) {
        Log::E("eglMakeCurrent() failed, err=%d\n", eglGetError());
        throw new Exception("eglMakeCurrent() failed");
    }

    if (!canvas_support_gl2()) {
        Log::E("GL2 is not supported\n", __func__);
        throw new Exception("canvas() failed");
    }

    canvas_init_extensions();
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    clear();
#ifdef EGL_DEBUG
    canvas_atrace(dpy, cfg);
#endif
}

Canvas::~Canvas()
{
    eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(dpy, ctx);
    eglDestroySurface(dpy, ctx);
    eglTerminate(dpy);
}

void Canvas::clear()
{
    glClearColor(clear_color.x(), clear_color.y(), clear_color.z(),
                 clear_color.w());
    glClearDepthf(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Canvas::resize(unsigned int w, unsigned int h)
{
    if(new_width != w || new_height != h) {
        new_resh = new_width = w;
        new_resv = new_height = h;
        resize_needed = true;
    }
}

void Canvas::update()
{
#ifdef __NGOS__
    if(resize_needed) {
        tGonkDisplay->SetResolution(new_resh, new_resv);
    }
    tGonkDisplay->SwapBuffers(dpy, srf);
    if(resize_needed) {
        glViewport(0, 0, new_resh, new_resv);

        canvas_width = new_width;
        canvas_height = new_height;
        canvas_resh = new_resh;
        canvas_resv = new_resv;
        resize_needed = false;
    }
#else
    eglSwapBuffers(dpy, srf);
#endif
}

unsigned int Canvas::width() const
{
    return canvas_width;
}

unsigned int Canvas::height() const
{
    return canvas_height;
}

void Canvas::setResolution(const Vector2i &resolution)
{
    if(new_resh != resolution.x() || new_resv != resolution.y()) {
        new_resh = resolution.x();
        new_resv = resolution.y();
        resize_needed = true;
    }
}

Vector2i Canvas::resolution() const
{
    return {canvas_resh, canvas_resv};
}

void Canvas::setClearColor(Vector4 const& color)
{
  clear_color = color;
}
