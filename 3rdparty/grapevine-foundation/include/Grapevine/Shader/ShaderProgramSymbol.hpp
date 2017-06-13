/**
 * Copyright (C) 2013-2016 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SHADER_SHADERPROGRAMSYMBOL
#define GRAPEVINE_SHADER_SHADERPROGRAMSYMBOL

#include "Shader.hpp"
#include <Grapevine/Math/Matrix.hpp>
#include <Grapevine/Math/Vector.hpp>
#include <Grapevine/GPU/GL.hpp>
#include <string>
#include <vector>
#include <map>

namespace Grapevine
{
    struct ShaderProgramSymbolType
    {
        enum Enum
        {
           TypeNone,
           TypeAttribute,
           TypeUniform
        };
    };

    /**
     * Shader program symbol class that abstracts a shader program symbol.
     */
    class ShaderProgramSymbol
    {
    public:
        /**
         * Constructor.
         *
         * @param name is the vertex shader source.
         * @param location is the vertex shader source.
         * @param type is the vertex shader source.
         */
        ShaderProgramSymbol(const std::string& name, int location, ShaderProgramSymbolType::Enum type);

        /**
         * Constructor.
         *
         * @param name is the vertex shader source.
         * @param location is the vertex shader source.
         * @param type is the vertex shader source.
         * @param glType is the vertex shader source.
         */
        ShaderProgramSymbol(const std::string& name, int location, ShaderProgramSymbolType::Enum type, GLenum glType);

        /**
         * Get symbol location.
         *
         * @return GL id of the shader symbol location.
         */
        int location() const;

        /**
         * Assign new value to shader symbol.
         *
         * @param new value.
         */
        ShaderProgramSymbol& operator=(const Matrix4& m);

        /**
         * Assign new value to shader symbol.
         *
         * @param new value.
         */
        ShaderProgramSymbol& operator=(const Vector2& v);

        /**
         * Assign new value to shader symbol.
         *
         * @param new value.
         */
        ShaderProgramSymbol& operator=(const Vector3& v);

        /**
         * Assign new value to shader symbol.
         *
         * @param new value.
         */
        ShaderProgramSymbol& operator=(const Vector4& v);

        /**
         * Assign new value to shader symbol.
         *
         * @param new value.
         */
        ShaderProgramSymbol& operator=(const float& f);

        /**
         * Assign new value to shader symbol.
         *
         * @param new value.
         */
        ShaderProgramSymbol& operator=(const int& i);

        /**
         * Assign new value to shader symbol.
         *
         * @param new value.
         */
        ShaderProgramSymbol& operator=(const std::vector<float> &v);

    private:
        ShaderProgramSymbol();
        ShaderProgramSymbolType::Enum type_;
        unsigned int location_;
        std::string name_;
        GLenum uniformType_;
    };
}

#endif

