/**
 * Copyright (C) 2013-2016 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SHADER_SHADERPROGRAM
#define GRAPEVINE_SHADER_SHADERPROGRAM

#include "ShaderProgramSymbol.hpp"
#include "ShaderSource.hpp"
#include "Shader.hpp"
#include <Grapevine/Math/Matrix.hpp>
#include <Grapevine/Math/Vector.hpp>
#include <string>
#include <vector>
#include <map>

namespace Grapevine
{
    /**
     * Shader program class that abstracts a GL shader program.
     */
    class ShaderProgram
    {
    public:
        /**
         * Some key values found from the shaders, use these freely if they are maching your needs.
         */
        static const std::string Normal;                // Normal, the vertex normal
        static const std::string Position;              // Position, the vertex position
        static const std::string TexCoord;              // TexCoord, the vertex u, v texture coordinate
        static const std::string ModelViewProjectionMatrix; // ModelViewProjectionMatrix
        static const std::string NormalMatrix;          // NormalMatrix, normal matrix
        static const std::string Opacity;               // Opacity, 0.0-1.0 opacity of the end color
        static const std::string Highlight;             // Highlight, 1.0-color "fat" factor for highlight
        static const std::string SpecularFactor;        // SpecularFactor, SpecularFactor multiplier
        static const std::string SpecularGlossiness;    // SpecularGlossiness, SpecularGlossiness power
        static const std::string Tangent;               // Normal Tangent
        static const std::string Bitangent;             // Normal Bitangent

        /**
         * Constructor with no shader program source.
         */
        ShaderProgram();

        /**
         * Constructor with vertex and fragment shader source files given.
         *
         * @param vertexShaderFilename is the vertex shader source.
         * @param fragmentShaderFilename is the fragment shader source.
         */
        ShaderProgram(char const* vertexShaderFilename, char const* fragmentShaderFilename);

        /**
         * Destructor. Marks the resources to be released. Call to purgeStaleShaders is needed to release resources.
         */
        ~ShaderProgram();

        /**
         * Adds shader to the program.
         *
         * @param shader to be added.
         */
        void addShader(ShaderSource& shader);

        /**
         * Attachs shader to the program.
         *
         * @param shader to be added.
         */
        void attach(Shader& program);

        /**
         * Get the id of the shader.
         *
         * @return GL shader program id.
         */
        unsigned int id() const;

        /**
         * Start shader. Binds the program.
         *
         * @note Shader must be in 'ready' state.
         */
        void start() const;

        /**
         * Stop shader.
         */
        void stop() const;

        /**
         * Return shader program symbol.
         *
         * @param name of the symbol.
         */
        ShaderProgramSymbol& operator[](char const* name);

        /**
         * Return shader program symbol.
         *
         * @param name of the symbol.
         */
        ShaderProgramSymbol& operator[](std::string const& name);

        /**
         * Build the shader from given Shader classes and/or source.
         *
         * @note minimum one shader must been added before calling this function. Program also needs to be in 'valid' state.
         */
        void build();

        /**
         * Check if shader is valid.
         *
         * @return TRUE if shader is valid. FALSE otherwise, use errorMessage() to see more information.
         */
        bool valid() const;

        /**
         * Check if shader is ready.
         *
         * @return TRUE if shader is ready and can be used. FALSE otherwise, use errorMessage() to see more information.
         */
        bool ready() const;

        /**
         * Returns the error message.
         *
         * @return error message.
         */
        std::string const& errorMessage() const;

        /**
         * Set uniform value.
         *
         * TODO: Check is this function duplicate to the ShaderProgramSymbol usage.
         *
         * @param id of the symbol.
         * @param value of the uniform.
         */
        void setUniform(int id, float value);

        /**
         * Get uniform's id.
         *
         * TODO: Check is this function duplicate to the ShaderProgramSymbol usage.
         *
         * @param name of the uniform to find.
         */
        int uniform(std::string const& name);

        /**
         * Releases resources of destructed ShaderPrograms.
         */
        static void purgeStaleShaderPrograms();

    private:
        static std::vector<unsigned int> staleShaderProgramIds_;

        std::string fileNames_; // NOTE: not available if created from the ShaderSources
        void addShader(ShaderType::Enum type, std::string const& source);
        void release();
        void init();
        int getAttribIndex(const std::string& name);
        int getUniformLocation(const std::string& name);
        unsigned int programId_;
        std::map<std::string, ShaderProgramSymbol*> symbols_;
        std::vector<Shader> shaders_;
        std::string message_;
        bool ready_;
        bool valid_;
    };
}

#endif

