/**
 * Copyright (C) 2013-2016 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <Grapevine/Resource/ResourceManagerPrivate.hpp>
#include "Grapevine/Shader/ShaderSource.hpp"
#include <Grapevine/Math/Vector.hpp>
#include <Grapevine/Math/Matrix.hpp>
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Core/Util.hpp>
#include <Grapevine/Core/Exception.hpp>
#include <string>
#include <sstream>
#include <vector>
#include <Grapevine/Core/Buffer.hpp>

namespace Grapevine
{
    /**
     * Holds default precision values for all shader types (also the unknown type)
     */
    std::vector<ShaderSource::Precision> ShaderSource::defaultPrecision_(ShaderType::Unknown + 1);

    ShaderSource::ShaderSource(ShaderType::Enum type):
        precisionSet_(false),
        type_(type),
        precision_()
    {
    }

    ShaderSource::ShaderSource(std::string const& filename, ShaderType::Enum type):
        precisionSet_(false),
        type_(type),
        precision_()
    {
        appendFile(filename);
    }

    void ShaderSource::loadFile(std::string const& filename, std::string& str)
    {
        Buffer* tmp = ResourceManagerPrivate::loadFile(filename.c_str());

        str.clear();
        str.reserve(tmp->byteSize() + 1);
        str.append(tmp->data(), tmp->byteSize());
        str += '\0';

        delete tmp;
    }

    void ShaderSource::appendFile(std::string const& filename)
    {
        std::string source;
        loadFile(filename, source);
        source_ << source;
    }

    void ShaderSource::addGlobal(const std::string &string)
    {
        std::string::size_type position = 0;
        std::string source(source_.str());

        // Find last precision qualifier
        position = source.rfind("precision");

        if (position != std::string::npos)
        {
            // Find next #endif line of a preprocessor block that contains precision
            // qualifier
            std::string::size_type positionIf = source.find("#if", position);
            std::string::size_type positionEndif = source.find("#endif", position);

            if (positionEndif != std::string::npos && positionEndif < positionIf)
            {
                position = positionEndif;
            }

            // Go to the next line
            position = source.find("\n", position);
            if (position != std::string::npos)
            {
                position++;
            }
        }
        else
        {
            position = 0;
        }

        source.insert(position, string);
        source_.clear();
        source_.str(source);
    }

    void ShaderSource::addLocal(const std::string &string,
        const std::string &function)
    {
        std::string::size_type position = 0;
        std::string source(source_.str());

        // Find the function
        position = source.find(function);
        position = source.find('{', position);

        // Next line
        position = source.find("\n", position);
        if (position != std::string::npos)
        {
            position++;
        }

        source.insert(position, string);
        source_.clear();
        source_.str(source);
    }

    void ShaderSource::add(const std::string &string, const std::string &function)
    {
        if (!function.empty())
        {
            addLocal(string, function);
        }
        else
        {
            addGlobal(string);
        }
    }

    /**
     * Adds a float constant definition.
     *
     * @name        Constant's name.
     * @value       Constant's value.
     * @function    Function if not empty. Function to put the definition in.
     */
    void ShaderSource::addConst(const std::string &name, float value,
        const std::string &function)
    {
        std::stringstream stream;
        stream << "const float " << name << " = " << std::fixed << value << ";" << std::endl;
        add(stream.str(), function);
    }


    /**
     * Adds a Vector4 constant definition.
     *
     * @param   Name of the constant
     * @param   Value of the constant
     * @param   Function if not empty, the function to put the definition in
     */
    void ShaderSource::addConst(const std::string &name, const Vector4 &v,
        const std::string &function)
    {
        std::stringstream stream;
        stream << "const vec4 " << name << " = vec4(" << std::fixed;
        stream << v.x() << ", " << v.y() << ", " << v.z() << ", " << v.w() << ");" << std::endl;
        add(stream.str(), function);
    }

    std::string ShaderSource::string(bool setPrecisions)
    {
        std::string precisionStr("");

        if (setPrecisions == true) {
            // Decide which precision values to use
            ShaderSource::Precision precision;

            if (precisionSet_) {
                precision = precision_;
            } else {
                precision = defaultPrecision(type());
            }

            // Create the precision statements
            std::stringstream ss;

            emitPrecision(ss, precision.intPrecision, "int");
            emitPrecision(ss, precision.floatPrecision, "float");
            emitPrecision(ss, precision.sampler2dPrecision, "sampler2D");
            emitPrecision(ss, precision.samplercubePrecision, "samplerCube");

            precisionStr = ss.str();
            if (!precisionStr.empty()) {
                precisionStr.insert(0, "#ifdef GL_ES\n");
                precisionStr.insert(precisionStr.size(), "#endif\n");
            }
        }

        return precisionStr + source_.str();
    }

    ShaderType::Enum ShaderSource::type()
    {
        // Try to get type by looking the source
        if (type_ == ShaderType::Unknown)
        {
            std::string source(source_.str());

            if (source.find("gl_FragColor") != std::string::npos)
            {
                type_ = ShaderType::Fragment;
            }
            else if (source.find("gl_Position") != std::string::npos)
            {
                type_ = ShaderType::Vertex;
            }
            else
            {
                Log::I("Cannot infer shader type from contents. Leaving it Unknown.\n");
            }
        }

        return type_;
    }

    const ShaderSource::Precision& ShaderSource::defaultPrecision(ShaderType::Enum type)
    {
        if (type < 0 || type > ShaderType::Unknown)
        {
            type = ShaderType::Unknown;
        }

        return defaultPrecision_[type];
    }

    /**
     * Helper function that emits a precision statement.
     *
     * @param ss the stringstream to add the statement to
     * @param val the precision value
     * @param typeStr the variable type to apply the precision value to
     */
    void ShaderSource::emitPrecision(std::stringstream& ss,
        ShaderSource::PrecisionValue val, const std::string& typeStr)
    {
        static const char *precision_map[] = { "lowp", "mediump", "highp", NULL };

        if (val == ShaderSource::PrecisionValueHigh) {
            if (type_ == ShaderType::Fragment) {
                ss << "#ifdef GL_FRAGMENT_PRECISION_HIGH" << std::endl;
            }

            ss << "precision highp " << typeStr << ";" << std::endl;

            if (type_ == ShaderType::Fragment) {
                ss << "#else" << std::endl;
                ss << "precision mediump " << typeStr << ";" << std::endl;
                ss << "#endif" << std::endl;
            }
        } else if (val >= 0 && val < ShaderSource::PrecisionValueDefault) {
            ss << "precision " << precision_map[val] << " ";
            ss << typeStr << ";" << std::endl;
        }

        // There is no default precision in the fragment shader-> set it to mediump
        if (val == ShaderSource::PrecisionValueDefault && typeStr == "float" && type_ == ShaderType::Fragment) {
            ss << "precision mediump float;" << std::endl;
        }
    }

    ShaderSource::Precision::Precision(const std::string& precisionValues):
        intPrecision(ShaderSource::PrecisionValueDefault),
        floatPrecision(ShaderSource::PrecisionValueDefault),
        sampler2dPrecision(ShaderSource::PrecisionValueDefault),
        samplercubePrecision(ShaderSource::PrecisionValueDefault)
    {
        std::vector<std::string> elems;
        Util::split(precisionValues, ',', elems);

        for (size_t i = 0; i < elems.size() && i < 4; i++) {
            const std::string& pString(elems[i]);
            ShaderSource::PrecisionValue pValue;

            if (pString == "high") {
                pValue = ShaderSource::PrecisionValueHigh;
            } else if (pString == "medium") {
                pValue = ShaderSource::PrecisionValueMedium;
            } else if (pString == "low") {
                pValue = ShaderSource::PrecisionValueLow;
            } else {
                pValue = ShaderSource::PrecisionValueDefault;
            }

            switch (i) {
                case 0: {
                    intPrecision = pValue;
                    break;
                }
                case 1: {
                    floatPrecision = pValue;
                    break;
                }
                case 2: {
                    sampler2dPrecision = pValue;
                    break;
                }
                case 3: {
                    samplercubePrecision = pValue;
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }
}
