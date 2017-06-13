/**
 * Copyright (C) 2013-2016 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SHADERS_SHADERSOURCE
#define GRAPEVINE_SHADERS_SHADERSOURCE

#include "ShaderType.hpp"
#include <Grapevine/Math/Vector.hpp>
#include <Grapevine/Math/Matrix.hpp>
#include <Grapevine/Core/String.hpp>
#include <string>
#include <sstream>
#include <vector>

namespace Grapevine
{
    /**
     * ShaderSource class to contain the shader source code.
     */
    class ShaderSource
    {
    public:
        /**
         * Constructor for shader source.
         *
         * @param type of the source.
         */
        ShaderSource(ShaderType::Enum type = ShaderType::Unknown);

        /**
         * Constructor for shader source.
         *
         * @param source for shader.
         * @param type of the source.
         */
        ShaderSource(std::string const& source, ShaderType::Enum type = ShaderType::Unknown);

        /**
         * Add function to shader source. Either global or local.
         *
         * TODO: bit misleading function, maybe to be split to two, i.e. using the setLocal and setGlobal straight?
         *
         * @param source to be set.
         * @param function name to set source into. If empty source is set to global space in source.
         *        @see setLocal() and @see setGlobal().
         */
        void add(const std::string &source, const std::string &function);
        void addConst(const std::string &name, float value, const std::string &function = "");
        void addConst(const std::string &name, const Vector4 &value,
            const std::string &function = "");

        /**
         * Load file to the end of current source.
         *
         * @param filename to be loaded.
         */
        void appendFile(std::string const& filename);

        /**
         * Load file to string.
         *
         * @param filename to be loaded.
         * @param target string to load the file to.
         */
        void loadFile(std::string const& filename, std::string& str);

        /**
         * Get full shader source code. Precisions are also added to to the source.
         *
         * @return the shader source in a string.
         */
        std::string string(bool setPrecisions = false);

        /**
         * Shader type.
         *
         * @return the shader source in a string.
         */
        ShaderType::Enum type();

        /**
         * Precision values.
         */
        enum PrecisionValue
        {
            PrecisionValueLow,
            PrecisionValueMedium,
            PrecisionValueHigh,
            PrecisionValueDefault
        };

        /**
         * Precision of different types.
         */
        struct Precision
        {
            Precision():
                intPrecision(PrecisionValueDefault),
                floatPrecision(PrecisionValueDefault),
                sampler2dPrecision(PrecisionValueDefault),
                samplercubePrecision(PrecisionValueDefault)
            {
            }

            Precision(PrecisionValue intP, PrecisionValue floatP,
                PrecisionValue sampler2dP, PrecisionValue samplercubeP):

                intPrecision(intP),
                floatPrecision(floatP),
                sampler2dPrecision(sampler2dP),
                samplercubePrecision(samplercubeP)
            {
            }

            Precision(const std::string& list);
            PrecisionValue intPrecision;
            PrecisionValue floatPrecision;
            PrecisionValue sampler2dPrecision;
            PrecisionValue samplercubePrecision;
        };

        static const Precision& defaultPrecision(ShaderType::Enum type);

    private:
        /**
         * Adds a source to source's global = placed after precision lines.
         *
         * @string Source to add.
         */
        void addGlobal(const std::string &source);

        /**
         * Adds a source to a function.
         *
         * @param source to add.
         * @param function to add the source into.
         */
        void addLocal(const std::string &source, const std::string &function);
        void emitPrecision(std::stringstream& ss, ShaderSource::PrecisionValue val, const std::string& type_str);

        std::stringstream source_;
        bool precisionSet_;
        ShaderType::Enum type_;
        Precision precision_;

        static std::vector<Precision> defaultPrecision_;
    };
}

#endif

