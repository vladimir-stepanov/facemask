/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_GPU_GL
#define GRAPEVINE_GPU_GL

#include <string>
#include <Grapevine/Core/Log.hpp>

#define GL_GLEXT_PROTOTYPES 1
#if __APPLE__ && !(TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE)
    #include <OpenGL/gl.h>
    #include <OpenGL/glext.h>
    #define glClearDepthf glClearDepth
    #define GL_APIENTRYP *
#else
    #include <EGL/egl.h>
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
#endif

namespace Grapevine
{
    #ifndef GL_WRITE_ONLY
        #define GL_WRITE_ONLY GL_WRITE_ONLY_OES
    #endif

    #if defined(__NGOS__) || defined(TSAIKKA)
        typedef uint64_t GLuint64;
        typedef int64_t GLint64;
    #endif

    #ifndef GL_EXT_disjoint_timer_query
    #define GL_EXT_disjoint_timer_query 1
        #define GL_QUERY_COUNTER_BITS_EXT                       0x8864
        #define GL_CURRENT_QUERY_EXT                            0x8865
        #define GL_QUERY_RESULT_EXT                             0x8866
        #define GL_QUERY_RESULT_AVAILABLE_EXT                   0x8867
        #define GL_TIME_ELAPSED_EXT                             0x88BF
        #define GL_TIMESTAMP_EXT                                0x8E28
        #define GL_GPU_DISJOINT_EXT                             0x8FBB

        void glGetQueryObjectivEXT(GLuint id, GLenum pname, GLint *params);
        void glGetQueryObjectuivEXT(GLuint id, GLenum pname, GLuint *params);
#if __ANDROID__
#else
        void glGetQueryObjecti64vEXT(GLuint id, GLenum pname, GLint64 *params);
        void glGetQueryObjectui64vEXT(GLuint id, GLenum pname, GLuint64 *params);
#endif
        typedef void (GL_APIENTRYP PFNGLGETQUERYOBJECTIVEXTPROC) (GLuint id, GLenum pname, GLint *params);
        typedef void (GL_APIENTRYP PFNGLGETQUERYOBJECTUIVEXTPROC) (GLuint id, GLenum pname, GLuint *params);
#if __ANDROID__
#else
        typedef void (GL_APIENTRYP PFNGLGETQUERYOBJECTI64VEXTPROC) (GLuint id, GLenum pname, GLint64 *params);
        typedef void (GL_APIENTRYP PFNGLGETQUERYOBJECTUI64VEXTPROC) (GLuint id, GLenum pname, GLuint64 *params);
#endif

        /* GL_EXT_occlusion_query_boolean */
        #ifndef GL_EXT_occlusion_query_boolean
            #define GL_EXT_occlusion_query_boolean 1
            #ifdef GL_GLEXT_PROTOTYPES
                void glGenQueriesEXT (GLsizei n, GLuint *ids);
                void glDeleteQueriesEXT (GLsizei n, const GLuint *ids);
                GLboolean glIsQueryEXT (GLuint id);
                void glBeginQueryEXT (GLenum target, GLuint id);
                void glEndQueryEXT (GLenum target);
                void glGetQueryivEXT (GLenum target, GLenum pname, GLint *params);
                void glGetQueryObjectuivEXT (GLuint id, GLenum pname, GLuint *params);
            #endif
            typedef void (GL_APIENTRYP PFNGLGENQUERIESEXTPROC) (GLsizei n, GLuint *ids);
            typedef void (GL_APIENTRYP PFNGLDELETEQUERIESEXTPROC) (GLsizei n, const GLuint *ids);
            typedef GLboolean (GL_APIENTRYP PFNGLISQUERYEXTPROC) (GLuint id);
            typedef void (GL_APIENTRYP PFNGLBEGINQUERYEXTPROC) (GLenum target, GLuint id);
            typedef void (GL_APIENTRYP PFNGLENDQUERYEXTPROC) (GLenum target);
            typedef void (GL_APIENTRYP PFNGLGETQUERYIVEXTPROC) (GLenum target, GLenum pname, GLint *params);
            typedef void (GL_APIENTRYP PFNGLGETQUERYOBJECTUIVEXTPROC) (GLuint id, GLenum pname, GLuint *params);
        #endif
    #endif

    // Functions in either GL2.0 or GLES2.0.
    struct GLExtensions
    {
        static void* (*MapBuffer) (GLenum target, GLenum access);
        static GLboolean (*UnmapBuffer) (GLenum target);

        static bool EXT_disjoint_timer_query;
        static PFNGLGETQUERYOBJECTIVEXTPROC GetQueryObjectivEXT;
        static PFNGLGETQUERYOBJECTUIVEXTPROC GetQueryObjectuivEXT;
#if __ANDROID__
#else
        static PFNGLGETQUERYOBJECTI64VEXTPROC GetQueryObjecti64vEXT;
        static PFNGLGETQUERYOBJECTUI64VEXTPROC GetQueryObjectui64vEXT;
#endif
        static PFNGLGENQUERIESEXTPROC GenQueriesEXT;
        static PFNGLDELETEQUERIESEXTPROC DeleteQueriesEXT;
        static PFNGLISQUERYEXTPROC IsQueryEXT;
        static PFNGLBEGINQUERYEXTPROC BeginQueryEXT;
        static PFNGLENDQUERYEXTPROC EndQueryEXT;
        static PFNGLGETQUERYIVEXTPROC GetQueryivEXT;
    };


    #ifndef GL_COMPRESSED_RGB8_ETC2
        #define GL_COMPRESSED_R11_EAC             0x9270
        #define GL_COMPRESSED_SIGNED_R11_EAC      0x9271
        #define GL_COMPRESSED_RG11_EAC            0x9272
        #define GL_COMPRESSED_SIGNED_RG11_EAC     0x9273
        #define GL_COMPRESSED_RGB8_ETC2           0x9274
        #define GL_COMPRESSED_SRGB8_ETC2          0x9275
        #define GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9276
        #define GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9277
        #define GL_COMPRESSED_RGBA8_ETC2_EAC      0x9278
        #define GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC 0x9279
    #endif

    #ifndef GL_ETC1_RGB8_OES
        #define GL_ETC1_RGB8_OES                  0x8D64
    #endif

    #ifndef GL_KHR_texture_compression_astc_hdr
        #define GL_KHR_texture_compression_astc_hdr 1
        #define GL_COMPRESSED_RGBA_ASTC_4x4_KHR   0x93B0
        #define GL_COMPRESSED_RGBA_ASTC_5x4_KHR   0x93B1
        #define GL_COMPRESSED_RGBA_ASTC_5x5_KHR   0x93B2
        #define GL_COMPRESSED_RGBA_ASTC_6x5_KHR   0x93B3
        #define GL_COMPRESSED_RGBA_ASTC_6x6_KHR   0x93B4
        #define GL_COMPRESSED_RGBA_ASTC_8x5_KHR   0x93B5
        #define GL_COMPRESSED_RGBA_ASTC_8x6_KHR   0x93B6
        #define GL_COMPRESSED_RGBA_ASTC_8x8_KHR   0x93B7
        #define GL_COMPRESSED_RGBA_ASTC_10x5_KHR  0x93B8
        #define GL_COMPRESSED_RGBA_ASTC_10x6_KHR  0x93B9
        #define GL_COMPRESSED_RGBA_ASTC_10x8_KHR  0x93BA
        #define GL_COMPRESSED_RGBA_ASTC_10x10_KHR 0x93BB
        #define GL_COMPRESSED_RGBA_ASTC_12x10_KHR 0x93BC
        #define GL_COMPRESSED_RGBA_ASTC_12x12_KHR 0x93BD
        #define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR 0x93D0
        #define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR 0x93D1
        #define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR 0x93D2
        #define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR 0x93D3
        #define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR 0x93D4
        #define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR 0x93D5
        #define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR 0x93D6
        #define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR 0x93D7
        #define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR 0x93D8
        #define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR 0x93D9
        #define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR 0x93DA
        #define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR 0x93DB
        #define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR 0x93DC
        #define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR 0x93DD
    #endif /* GL_KHR_texture_compression_astc_hdr */

    void _check_gl_error(const char *file, int line);

    void glBindBuffer(  GLenum target, GLuint buffer);

    #define LOGGLERROR _check_gl_error(__FILE__, __LINE__);
}

#endif
