/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SHADER_SHADERMANAGER
#define GRAPEVINE_SHADER_SHADERMANAGER

#include "Grapevine/Shader/ShaderProgram.hpp"

namespace Grapevine
{
    class ShaderManagerPrivate;

    class ShaderManager
    {
    public:
        ShaderManager();
        ~ShaderManager();
        static void setDefaultShaderProgram(ShaderProgram* program);
        static ShaderProgram* defaultShaderProgram();

    private:
        static ShaderManagerPrivate* singleton_;
    };
}

#endif
