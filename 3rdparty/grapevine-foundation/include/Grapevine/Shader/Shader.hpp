/**
 * Copyright (C) 2013-2016 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SHADER_SHADER
#define GRAPEVINE_SHADER_SHADER

#include "ShaderType.hpp"
#include <string>
#include <vector>

namespace Grapevine
{
    /**
     * Class for GPU shaders to be linked to a ShaderProgram.
     */
    class Shader
    {
    public:
        /**
         * Shader copy constructor.
         */
        Shader(const Shader& shader);

        /**
         * Shader constructor.
         *
         * @param type of the shader.
         * @param source for the shader.
         */
        Shader(ShaderType::Enum type, std::string const& source);
        ~Shader();

        /**
         * Compiles shader so it can be linked into a full shader program.
         */
        void compile();

        /**
         * Marks the resources to be released. Call to purgeStaleShaders() is needed to release resources.
         */
        void release();

        /**
         * Checks is source set.
         *
         * @return TRUE if source is set. FALSE otherwise.
         */
        bool sourceSet() const;

        /**
         * Checks if shader is compiled.
         *
         * @return TRUE if source is compiled. FALSE otherwise.
         */
        bool compiled() const;

        /**
         * Releases resources of destructed Shaders.
         */
        static void purgeStaleShaders();

    private:
        friend class ShaderProgram;
        static std::vector<unsigned int> staleShaderHandles_;
        unsigned int handle_;
        ShaderType::Enum type_;
        std::string source_;
        std::string message_;
        bool compiled_;
        bool sourceSet_;
    };
}

#endif
