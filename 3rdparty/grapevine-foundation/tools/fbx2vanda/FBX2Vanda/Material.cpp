// This file is part of FBX2Vanda project. Read README for more information.
// Author: peuhkura@gmail.com

#include "FBX2Vanda/Material.hpp"
#include <stdint.h>
#include <Grapevine/Core/Exception.hpp>

namespace FBX2Vanda
{
    #define TOO_LONG_FILENAME "You are using quite long filenames.`They say that this has something to do with lack of sex."

    Material::Material():
        specularFactor_(0.5f),
        reflectionFactor_(0.5f),
        materialID_(-1)
    {
    }

    Material::~Material()
    {
    }

    void Material::setMaterialName(std::string const& name)
    {
        materialName_ = name;
    }

    std::string* Material::getMaterialName(void)
    {
        return &materialName_;
    }

    void Material::setMaterialID(int id)
    {
        materialID_ = id;
    }

    void Material::setColorMapFile(std::string const& textureName)
    {
        colorMapFilename_ = std::string(textureName);
    }

    void Material::setNormalMapFile(std::string const& textureName)
    {
        normalMapFilename_ = std::string(textureName);
    }

    void Material::setSpecularFactor(float specularFactor)
    {
        specularFactor_ = specularFactor;
    }

    void Material::setGlossinessFactor(float glossinessFactor)
    {
        glossinessFactor_ = glossinessFactor;
    }

    void Material::setReflectionFactor(float factor)
    {
        reflectionFactor_ = factor;
    }

    void Material::write(std::ofstream& fileStream)
    {
        // write header
        /*LOGD("write name");
        {
            if (materialName_.size() > 255)
            {
                LOGE(TOO_LONG_FILENAME);
                throw std::exception();
            }

            // 8 bit uint: name size, if 0 no name was given
            uint8_t nameSize = materialName_.size();
            Grapevine::Log::D("nameSize: %d", nameSize);

            LOGD("name: %s", materialName_.c_str());
            fileStream.write(reinterpret_cast<char const*>(&nameSize), sizeof(nameSize));

            if (nameSize > 0)
            {
                // N bytes: name
                fileStream << materialName_;
            }
        }*/

        // material ID
        fileStream.write(reinterpret_cast<char const*>(&materialID_), sizeof(materialID_));

        LOGD("write colormap filename");
        {
            if (colorMapFilename_.size() > 255)
            {
                LOGE(TOO_LONG_FILENAME);
                throw std::exception();
            }

            // 8 bit uint: name size, if 0 no colormap was given
            uint8_t nameSize = colorMapFilename_.size();
            Grapevine::Log::D("nameSize: %d", nameSize);

            LOGD("name: %s", colorMapFilename_.c_str());
            fileStream.write(reinterpret_cast<char const*>(&nameSize), sizeof(nameSize));

            if (nameSize > 0)
            {
                // N bytes: name
                fileStream << colorMapFilename_;
            }
        }

        LOGD("write normalmap filename");
        {
            if (normalMapFilename_.size() > 255)
            {
                LOGE(TOO_LONG_FILENAME);
                throw std::exception();
            }

            // 8 bit uint: name size, if 0 no name was given
            uint8_t nameSize = normalMapFilename_.size();
            Grapevine::Log::D("nameSize: %d", nameSize);

            LOGD("name: %s", normalMapFilename_.c_str());
            fileStream.write(reinterpret_cast<char const*>(&nameSize), sizeof(nameSize));

            if (nameSize > 0)
            {
                // N bytes: name
                fileStream << normalMapFilename_;
            }
        }

        LOGD("write specular factor: %f", specularFactor_)
        fileStream.write(reinterpret_cast<char*>(&specularFactor_), sizeof(float));

        LOGD("write glossiness factor: %f", glossinessFactor_)
        fileStream.write(reinterpret_cast<char*>(&glossinessFactor_), sizeof(float));

        LOGD("write reflection factor: %f", reflectionFactor_)
        fileStream.write(reinterpret_cast<char*>(&reflectionFactor_), sizeof(float));
    }
}
