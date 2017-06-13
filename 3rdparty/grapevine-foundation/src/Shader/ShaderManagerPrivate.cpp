/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "ShaderManagerPrivate.hpp"

namespace Grapevine
{
    ShaderManagerPrivate::ShaderManagerPrivate():
        defaultShaderProgram_(NULL)
    {
    }

    ShaderManagerPrivate::~ShaderManagerPrivate()
    {
        if (defaultShaderProgram_ != NULL)
        {
            // TODO: RAII for shader program
        }
    }

    void ShaderManagerPrivate::setDefaultShaderProgram(ShaderProgram* program)
    {
        if (defaultShaderProgram_ != NULL)
        {
            // TODO: RAII for shader program
        }
        defaultShaderProgram_ = program;
    }

    ShaderProgram* ShaderManagerPrivate::defaultShaderProgram()
    {
        return defaultShaderProgram_;
    }
}
