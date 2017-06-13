/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Shader/ShaderProgramSymbol.hpp"

namespace Grapevine
{
    ShaderProgramSymbol::ShaderProgramSymbol(const std::string& name, int location,
        ShaderProgramSymbolType::Enum type):
        type_(type),
        location_(location),
        name_(name),
        uniformType_(GL_FALSE)
    {
    }

    ShaderProgramSymbol::ShaderProgramSymbol(
        const std::string& name,
        int location,
        ShaderProgramSymbolType::Enum type,
        GLenum glType)
    :
        type_(type),
        location_(location),
        name_(name),
        uniformType_(glType)
    {

    }

    int ShaderProgramSymbol::location() const
    {
        return location_;
    }

    ShaderProgramSymbol& ShaderProgramSymbol::operator=(const Matrix4& m)
    {
        if (type_ == ShaderProgramSymbolType::TypeUniform)
        {
            // Matrix column major-> transpose false
            glUniformMatrix4fv(location_, 1, GL_FALSE, m);
        }
        return *this;
    }

    ShaderProgramSymbol& ShaderProgramSymbol::operator=(const Vector2& v)
    {
        if (type_ == ShaderProgramSymbolType::TypeUniform)
        {
            glUniform2fv(location_, 1, v);
        }
        return *this;
    }

    ShaderProgramSymbol& ShaderProgramSymbol::operator=(const Vector3& v)
    {
        if (type_ == ShaderProgramSymbolType::TypeUniform)
        {
            glUniform3fv(location_, 1, v);
        }
        return *this;
    }

    ShaderProgramSymbol& ShaderProgramSymbol::operator=(const Vector4& v)
    {
        if (type_ == ShaderProgramSymbolType::TypeUniform)
        {
            glUniform4fv(location_, 1, v);
        }
        return *this;
    }

    ShaderProgramSymbol& ShaderProgramSymbol::operator=(const float& f)
    {
        if (type_ == ShaderProgramSymbolType::TypeUniform)
        {
            glUniform1f(location_, f);
        }
        return *this;
    }

    ShaderProgramSymbol& ShaderProgramSymbol::operator=(const int& i)
    {
        if (type_ == ShaderProgramSymbolType::TypeUniform)
        {
            glUniform1i(location_, i);
        }
        return *this;
    }

    ShaderProgramSymbol& ShaderProgramSymbol::operator=(const std::vector<float> &v)
    {
        if (type_ == ShaderProgramSymbolType::TypeUniform)
        {
            int size = v.size();

            switch(uniformType_)
            {
                case GL_FLOAT:
                    glUniform1fv(location_, size, reinterpret_cast<const GLfloat *>(v.data()));
                break;
                case GL_FLOAT_VEC2:
                    glUniform2fv(location_, size/2, reinterpret_cast<const GLfloat *>(v.data()));
                break;
                case GL_FLOAT_VEC3:
                    glUniform3fv(location_, size/3, reinterpret_cast<const GLfloat *>(v.data()));
                break;
                case GL_FLOAT_VEC4:
                    glUniform4fv(location_, size/4, reinterpret_cast<const GLfloat *>(v.data()));
                break;
                default:
                    LOGE("No valid uniform type set for this operator 0x%x",uniformType_);
                break;
            }
        }

        return *this;
    }
}
