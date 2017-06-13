/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <Grapevine/Resource/ResourceManagerPrivate.hpp>
#include "Grapevine/Material/CubemapResource.hpp"
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/GPU/GL.hpp>
#include "../Image/ImageResource.hpp"

namespace Grapevine
{
    CubemapResource::CubemapResource(std::string const& filename, CubemapAsset::Type::Enum type):
        Resource(filename),
        imageResource_(NULL),
        textureId_(0),
        type_(type)
    {
        // We need ImageResource as source to the cubemap
        std::string id = "image" + filename;
        imageResource_ = dynamic_cast<ImageResource*>(ResourceManagerPrivate::resource(id));
        if (imageResource_ == NULL)
        {
            imageResource_ = new ImageResource(filename);
            ResourceManagerPrivate::setResource(id, imageResource_);
        }
        imageResource_->increment();
    }

    CubemapResource::CubemapResource(Bitmap& bitmap, CubemapAsset::Type::Enum type):
        Resource(""),
        imageResource_(new ImageResource(bitmap)),
        textureId_(0),
        type_(type)
    {
        imageResource_->increment();
    }

    CubemapResource::~CubemapResource()
    {
        if (textureId_ != 0)
        {
            staleTextureIds_.push_back(textureId_);
        }

        if (imageResource_ != NULL)
        {
            imageResource_->decrement();
            imageResource_ = NULL;
        }
    }

    unsigned int CubemapResource::textureId()
    {
        if (textureId_ == 0)
        {
            initialize();
        }
        return textureId_;
    }

    void CubemapResource::initialize()
    {
        GLenum format = GL_RGBA;
        switch(imageResource_->format())
        {
            case Bitmap::Format::LUMINANCE_8:
            {
                format = GL_LUMINANCE;
                break;
            }
            case Bitmap::Format::RGBA_8888:
            {
                format = GL_RGBA;
                break;
            }
            case Bitmap::Format::RGB_888:
            {
                format = GL_RGB;
                break;
            }
            default:
            {
                LOGE("Format is not supported. Using GL_RGBA, results are unknown!");
            }
        }

        // Generate a texture object
        glGenTextures(1, &textureId_);

        // Bind the texture object
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureId_);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        if (type_ == CubemapAsset::Type::HorizontalCross)
        {
            throw Exception("No support for type HorizontalCross, yet.");
        }
        else if (type_ == CubemapAsset::Type::Strip) // order: front, left, back, right, top, bottom
        {
            unsigned int side = imageResource_->height();
            if (side * 6 != imageResource_->width())
            {
                throw Exception("CubemapResource::initialize(), image width must be equal to 6x image height.");
            }

            ImageResource posZ(*imageResource_, 0 * side, 0, side, side);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, side, side, 0, format, GL_UNSIGNED_BYTE, posZ.pixels());

            ImageResource negX(*imageResource_, 1 * side, 0, side, side);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, side, side, 0, format, GL_UNSIGNED_BYTE, negX.pixels());

            ImageResource negZ(*imageResource_, 2 * side, 0, side, side);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, side, side, 0, format, GL_UNSIGNED_BYTE, negZ.pixels());

            ImageResource posX(*imageResource_, 3 * side, 0, side, side);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, side, side, 0, format, GL_UNSIGNED_BYTE, posX.pixels());

            ImageResource posY(*imageResource_, 4 * side, 0, side, side);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, side, side, 0, format, GL_UNSIGNED_BYTE, posY.pixels());

            ImageResource negY(*imageResource_, 5 * side, 0, side, side);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, side, side, 0, format, GL_UNSIGNED_BYTE, negY.pixels());
        }

        imageResource_->decrement();
        imageResource_ = NULL;
    }

    void CubemapResource::purgeStaleCubemaps()
    {
        if(!staleTextureIds_.empty())
        {
            glDeleteTextures(staleTextureIds_.size(), &staleTextureIds_[0]);
            staleTextureIds_.clear();
        }
    }

    std::vector<unsigned int> CubemapResource::staleTextureIds_;
}
