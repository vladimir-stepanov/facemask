/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SHADER_SHADERMANAGERPRIVATE
#define GRAPEVINE_SHADER_SHADERMANAGERPRIVATE

#include "Grapevine/Shader/ShaderProgram.hpp"

namespace Grapevine
{
    class ShaderManagerPrivate
    {
    public:
        void setDefaultShaderProgram(ShaderProgram* program);
        ShaderProgram* defaultShaderProgram();

    private:
        friend class ShaderManager;
        ShaderManagerPrivate();
        ~ShaderManagerPrivate();
        ShaderProgram* defaultShaderProgram_;
    };
}

#endif
