/**
 * Copyright (C) 2013-2016 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Shader/Shader.hpp"
#include "Grapevine/Shader/ShaderProgram.hpp"
#include "Grapevine/Shader/ShaderProgramSymbol.hpp"
#include "Grapevine/Shader/VertexSource.hpp"
#include "Grapevine/Shader/FragmentSource.hpp"
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/GPU/GL.hpp>
#include <Grapevine/Core/Exception.hpp>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

namespace Grapevine
{
    // Constants
    const std::string ShaderProgram::Normal = std::string("Normal");
    const std::string ShaderProgram::Position = std::string("Position");
    const std::string ShaderProgram::TexCoord = std::string("TexCoord");
    const std::string ShaderProgram::ModelViewProjectionMatrix = std::string("ModelViewProjectionMatrix");
    const std::string ShaderProgram::NormalMatrix = std::string("NormalMatrix");
    const std::string ShaderProgram::Opacity = std::string("Opacity");
    const std::string ShaderProgram::Highlight = std::string("Highlight");
    const std::string ShaderProgram::SpecularFactor = std::string("SpecularFactor");
    const std::string ShaderProgram::SpecularGlossiness = std::string("SpecularGlossiness");
    const std::string ShaderProgram::Tangent = std::string("Tangent");
    const std::string ShaderProgram::Bitangent = std::string("Bitangent");

    bool gotSource(const std::string& filename, std::string& source)
    {
        using std::ifstream;
        ifstream inputFile(filename.c_str());
        if (!inputFile) {
            Log::E("Failed to open %s", filename.c_str());
            return false;
        }

        std::string curLine;
        while (getline(inputFile, curLine)) {
            source += curLine;
            source += '\n';
        }

        return true;
    }

    ShaderProgram::ShaderProgram():
        fileNames_(""),
        programId_(0),
        ready_(false),
        valid_(false)
    {
        init();
    }

    ShaderProgram::ShaderProgram(char const* vertexShaderFilename, char const* fragmentShaderFilename):
        fileNames_(std::string("vertex: ") + std::string(vertexShaderFilename) + std::string(", fragment: ") + std::string(fragmentShaderFilename)),
        programId_(0),
        ready_(false),
        valid_(false)
    {
        VertexSource vertex(vertexShaderFilename);
        FragmentSource fragment(fragmentShaderFilename);
        init();
        addShader(vertex);
        addShader(fragment);
        build();
    }

    ShaderProgram::~ShaderProgram()
    {
        release();
    }

    void ShaderProgram::attach(Shader& shader)
    {
        if (!(shader.sourceSet()) || !(shader.compiled())) {
            Log::E("Shader attach failed: sourceSet:%d compiled:%d", shader.sourceSet(), shader.compiled());
            throw Exception("Shader attach failed.");
        }

        glAttachShader(programId_, shader.handle_);
    }

    void ShaderProgram::addShader(ShaderSource& source)
    {
        if (!valid_) {
            throw Exception("ShaderProgram is not valid.");
        }

        ShaderType::Enum type = source.type();
        std::string sourceLines = source.string();
        Shader shader(type, sourceLines);
        shader.compile();
        glAttachShader(programId_, shader.handle_);
        shaders_.push_back(shader);
        return;
    }

    /**
     * Initialize
     */
    void ShaderProgram::init()
    {
        programId_ = glCreateProgram();
        if (!programId_) {
            message_ = std::string("Failed to create the new program");
            return;
        }

        valid_ = true;
    }

    void ShaderProgram::release()
    {
        // First delete all of the shader resources attached to us.
        for (std::vector<Shader>::iterator shaderIterator = shaders_.begin();
            shaderIterator != shaders_.end(); shaderIterator++) {

            shaderIterator->release();
        }

        // Clear out the shader vector so we're ready to reuse it.
        shaders_.clear();

        // Clear error std::string
        message_.clear();

        // Release all of the symbol map resources.
        for (std::map<std::string, ShaderProgramSymbol*>::iterator symbolIterator =
            symbols_.begin(); symbolIterator != symbols_.end(); symbolIterator++) {

            delete (*symbolIterator).second;
        }
        symbols_.clear();

        if (programId_)
        {
            staleShaderProgramIds_.push_back(programId_);
        }
        programId_ = 0;
        ready_ = false;
        valid_ = false;
    }

    void ShaderProgram::addShader(ShaderType::Enum type, const std::string& source)
    {
        if (!valid_) {
            return;
        }

        Shader shader(type, source);
        if (!shader.sourceSet()) {
            valid_ = false;
            return;
        }

        shader.compile();

        if (!shader.compiled()) {
            valid_ = false;
            return;
        }

        glAttachShader(programId_, shader.handle_);
        shaders_.push_back(shader);
        return;
    }

    void ShaderProgram::build()
    {
        static bool errorShown = false;

        if (!valid_) {
            LOGGLERROR
            throw Exception("ShaderProgram not valid!");
        }

        if (shaders_.empty()) {
            message_ = std::string("There are no shaders attached to this program");
            return;
        }

        glLinkProgram(programId_);
        GLint param = 1;
        glGetProgramiv(programId_, GL_LINK_STATUS, &param);
        if (param == GL_FALSE && errorShown == false) {
            glGetProgramiv(programId_, GL_INFO_LOG_LENGTH, &param);
            char* infoLog = new char[param + 1];
            glGetProgramInfoLog(programId_, param + 1, NULL, infoLog);
            message_ = infoLog;
            LOGE("Shader::compile(), glLinkProgram() & GL_LINK_STATUS: \"%s\"", message_.c_str());
            delete [] infoLog;
            errorShown = true;
            return;
        }

        // find uniforms
        int uniformCount = 0;
        glGetProgramiv( programId_, GL_ACTIVE_UNIFORMS, &uniformCount );

        char name[256];
        int nameLen;
        int num;
        GLenum type;

        for(int i=0; i<uniformCount; ++i) {

            glGetActiveUniform( programId_, GLuint(i), sizeof(name)-1, &nameLen, &num, &type, name);
            name[nameLen] = 0;

            for(int i=0;i<nameLen;i++) {
                if(name[i] == 0x5B) {
                    name[i] = 0;
                    break;
                }
            }

            GLuint location = glGetUniformLocation( programId_, name );

            symbols_[name] = new ShaderProgramSymbol(name, location, ShaderProgramSymbolType::TypeUniform, type);
        }

        ready_ = true;
    }

    void ShaderProgram::start() const
    {
        if (!valid_) {
            throw Exception("ShaderProgram not valid!");
        }
        else if (!ready_) {
            throw Exception("ShaderProgram not ready!");
        }

        glUseProgram(programId_);
    }

    /**
     * Stop using program.
     */
    void ShaderProgram::stop() const
    {
        glUseProgram(0);
    }

    unsigned int ShaderProgram::id() const
    {
        return programId_;
    }

    int ShaderProgram::uniform(std::string const& name)
    {
        const char *tmp = name.c_str();
        GLint location = glGetUniformLocation(programId_, tmp);
        if (location < 0)
        {
            message_ = std::string("Failed to get uniform location for \"") + name + std::string("\"");
        }
        return location;
    }

    void ShaderProgram::setUniform(int id, float value)
    {
        glUniform1f(id ,value);
    }

    int ShaderProgram::getUniformLocation(std::string const& name)
    {
        GLint location = glGetUniformLocation(programId_, name.c_str());
        if (location < 0) {
            message_ = std::string("Failed to get uniform location for \"") + name + std::string("\"");
        }
        return location;
    }

    int ShaderProgram::getAttribIndex(std::string const& name)
    {
        GLint index = glGetAttribLocation(programId_, name.c_str());
        if (index < 0) {
            message_ = std::string("Failed to get attribute location for \"") + name + std::string("\"");
        }
        return index;
    }

    /**
     * Return symbol with given name
     *
     * @param tmp is the given string
     * @return symbol matching the name
     */
    ShaderProgramSymbol& ShaderProgram::operator[](std::string const& name)
    {
        std::map<std::string, ShaderProgramSymbol*>::iterator mapIterator = symbols_.find(name);

        if (mapIterator == symbols_.end()) {
            ShaderProgramSymbolType::Enum type(ShaderProgramSymbolType::TypeAttribute);
            int location = getAttribIndex(name);
            if (location < 0) {
                // No attribute found by that name, try a uniform.
                type = ShaderProgramSymbolType::TypeUniform;
                location = getUniformLocation(name);
                if (location < 0) {
                    LOGI("Symbol \"%s\" not found from shader source code. This symbol is not used.", name.c_str(), fileNames_.c_str());
                }
            }

            mapIterator = symbols_.insert(mapIterator, std::make_pair(name,
                new ShaderProgramSymbol(name, location, type)));
        }

        return *(*mapIterator).second;
    }

    ShaderProgramSymbol& ShaderProgram::operator[](char const* name)
    {
        std::string tmp(name);
        return (*this)[tmp];
    }

    bool ShaderProgram::valid() const
    {
        return valid_;
    }

    bool ShaderProgram::ready() const
    {
        return ready_;
    }

    const std::string& ShaderProgram::errorMessage() const
    {
        return message_;
    }

    void ShaderProgram::purgeStaleShaderPrograms()
    {
        for(auto programId: staleShaderProgramIds_) {
            glDeleteProgram(programId);
        }
        staleShaderProgramIds_.clear();
    }

    std::vector<unsigned int> ShaderProgram::staleShaderProgramIds_;
}
