/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/GPU/GL.hpp"

namespace Grapevine
{
    void* (*GLExtensions::MapBuffer) (GLenum target, GLenum access) = 0;
    GLboolean (*GLExtensions::UnmapBuffer) (GLenum target) = 0;

    bool GLExtensions::EXT_disjoint_timer_query = false;
    PFNGLGETQUERYOBJECTIVEXTPROC GLExtensions::GetQueryObjectivEXT = 0;
    PFNGLGETQUERYOBJECTUIVEXTPROC GLExtensions::GetQueryObjectuivEXT = 0;
#if __ANDROID__
#else
    PFNGLGETQUERYOBJECTI64VEXTPROC GLExtensions::GetQueryObjecti64vEXT = 0;
    PFNGLGETQUERYOBJECTUI64VEXTPROC GLExtensions::GetQueryObjectui64vEXT = 0;
#endif
    PFNGLGENQUERIESEXTPROC GLExtensions::GenQueriesEXT = 0;
    PFNGLDELETEQUERIESEXTPROC GLExtensions::DeleteQueriesEXT = 0;
    PFNGLISQUERYEXTPROC GLExtensions::IsQueryEXT = 0;
    PFNGLBEGINQUERYEXTPROC GLExtensions::BeginQueryEXT = 0;
    PFNGLENDQUERYEXTPROC GLExtensions::EndQueryEXT = 0;
    PFNGLGETQUERYIVEXTPROC GLExtensions::GetQueryivEXT = 0;

    void _check_gl_error(const char *file, int line)
    {
        GLenum err(glGetError());
        if (err == GL_NO_ERROR)
        {
            LOGE("GL error: GL_NO_ERROR");
            return;
        }

        while (err != GL_NO_ERROR)
        {
            std::string error;

            switch (err)
            {
                case GL_INVALID_OPERATION: error="INVALID_OPERATION"; break;
                case GL_INVALID_ENUM: error="INVALID_ENUM"; break;
                case GL_INVALID_VALUE: error="INVALID_VALUE"; break;
                case GL_OUT_OF_MEMORY: error="OUT_OF_MEMORY"; break;
                case GL_INVALID_FRAMEBUFFER_OPERATION: error="INVALID_FRAMEBUFFER_OPERATION"; break;
            }

            LOGE("GL error: %s, file: %s, line: %d");
            err = glGetError();
        }
    }

    void glBindBuffer(GLenum target, GLuint buffer)
    {
        static GLenum prevTarget = 0x0;
        static GLuint prevBuffer = 0xFFFFFFFF;

        if(prevTarget != target || prevBuffer != buffer)
        {
            prevTarget = target;
            prevBuffer = buffer;
            ::glBindBuffer(target, buffer);
        }
    }
}
