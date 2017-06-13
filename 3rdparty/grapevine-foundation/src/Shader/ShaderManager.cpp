/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "ShaderManager.hpp"
#include "ShaderManagerPrivate.hpp"
#include <Grapevine/Core/Exception.hpp>

namespace Grapevine
{
    ShaderManagerPrivate* ShaderManager::singleton_ = NULL;

    ShaderManager::ShaderManager()
    {
        if (singleton_ == NULL)
        {
            singleton_ = new ShaderManagerPrivate();
        }
        else
        {
            throw Exception("ShaderManagerPrivate exists already, you can have only one ShaderManager");
        }
    }

    ShaderManager::~ShaderManager()
    {
        if (singleton_ != NULL)
        {
            delete singleton_;
            singleton_ = NULL;
        }
    }

    void ShaderManager::setDefaultShaderProgram(ShaderProgram* program)
    {
        singleton_->setDefaultShaderProgram(program);
    }

    ShaderProgram* ShaderManager::defaultShaderProgram()
    {
        return singleton_->defaultShaderProgram();
    }
}
