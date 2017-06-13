/**
 * Copyright (C) 2013-2016 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Shader/Shader.hpp"
#include <Grapevine/GPU/GL.hpp>
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Core/Exception.hpp>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>

namespace Grapevine
{
    Shader::Shader(ShaderType::Enum type, std::string const& source):
        handle_(0),
        type_(type),
        source_(source),
        compiled_(false),
        sourceSet_(false)
    {
        int tmp = -1;
        if (type == ShaderType::Vertex) {
            tmp = GL_VERTEX_SHADER;
        }
        else if (type == ShaderType::Fragment) {
            tmp = GL_FRAGMENT_SHADER;
        }

        handle_ = glCreateShader(tmp);
        if (!handle_) {
            throw Exception("Failed to create the new shader.");
            return;
        }
#ifdef __APPLE__
        const char* shaderSource[2];
        shaderSource[0] = "#define precision\n"
                          "#define highp\n"
                          "#define mediump\n"
                          "#define lowp\n";
        shaderSource[1] = source_.c_str();
        glShaderSource(handle_, 2, &shaderSource[0], NULL);
#else
        const char* shaderSource = source_.c_str();
        glShaderSource(handle_, 1, &shaderSource, NULL);
#endif
        sourceSet_ = true;
    }

    Shader::Shader(const Shader& shader):
        handle_(shader.handle_),
        type_(shader.type_),
        source_(shader.source_),
        message_(shader.message_),
        compiled_(shader.compiled_),
        sourceSet_(shader.sourceSet_)
    {
    }

    Shader::~Shader()
    {
        handle_ = 0;
        type_ = ShaderType::Unknown;
        compiled_ = false;
        sourceSet_ = false;
    }

    void Shader::compile()
    {
        static bool errorShown = false;

        if (!sourceSet_) {
            throw Exception("Shader compile failed, not valid for compilation!");
        }

        glCompileShader(handle_);
        GLint param = 0;
        glGetShaderiv(handle_, GL_COMPILE_STATUS, &param);
        if (param == GL_FALSE && errorShown == false) {
            glGetShaderiv(handle_, GL_INFO_LOG_LENGTH, &param);
            char* infoLog = new char[param + 1];
            glGetShaderInfoLog(handle_, param + 1, NULL, infoLog);
            message_ = infoLog;
            delete [] infoLog;
            LOGE("Shader::compile(), glCompileShader & GL_COMPILE_STATUS: \"%s\"", message_.c_str());
            errorShown = true;
        }
        compiled_ = true;
    }

    void Shader::release()
    {
        if (handle_) {
            staleShaderHandles_.push_back(handle_);
        }
        handle_ = 0;
        type_ = ShaderType::Unknown;
        compiled_ = false;
        sourceSet_ = false;
    }

    bool Shader::sourceSet() const
    {
        return sourceSet_;
    }

    bool Shader::compiled() const
    {
        return compiled_;
    }

    void Shader::purgeStaleShaders()
    {
        for (auto handle: staleShaderHandles_) {
            glDeleteShader(handle);
        }
        staleShaderHandles_.clear();
    }

    std::vector<unsigned int> Shader::staleShaderHandles_;
}
