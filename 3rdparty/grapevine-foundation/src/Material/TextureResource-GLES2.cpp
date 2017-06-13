/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <Grapevine/Resource/ResourceManagerPrivate.hpp>
#include "Grapevine/Material/TextureResource.hpp"
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/GPU/GL.hpp>
#include "../Image/ImageResource.hpp"
#include "../Image/GeneralImageResource.hpp"
#include "Grapevine/Animation/AnimationManager.hpp"

#ifndef GL_TEXTURE_EXTERNAL_OES
// assume that platforms without GL_TEXTURE_EXTERNAL_OES will not try to make use of it either
#define GL_TEXTURE_EXTERNAL_OES -1
#endif

namespace Grapevine
{
    unsigned int createTexture(bool mipmaps, unsigned int width, unsigned int height, GLenum format, char* source)
    {
        unsigned int id = 0;
        GLint minFilter = GL_LINEAR;
        GLint magFilter = GL_LINEAR;
        if(mipmaps) {
            minFilter = GL_LINEAR_MIPMAP_NEAREST;
        }
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D,
            0,
            format,
            width,
            height,
            0,
            format,
            GL_UNSIGNED_BYTE,
            source);
        if(mipmaps) {
            if((minFilter != GL_NEAREST && minFilter != GL_LINEAR)
                ||
                (magFilter != GL_NEAREST && magFilter != GL_LINEAR)) {
                glGenerateMipmap(GL_TEXTURE_2D);
            }
            else {
                LOGE("Error in texture filters! Texture not created!");
            }
        }

        LOGD("createTexture GL texture: %ix%i, id: %d", width, height, id);

        glBindTexture(GL_TEXTURE_2D,0);

        return id;
    }

    unsigned int createCompressedTexture(uint32_t internalFormat, uint32_t width, uint32_t height, uint32_t size, const uint8_t *data)
    {
        unsigned int id = 0;
        GLint minFilter = GL_LINEAR;
        GLint magFilter = GL_LINEAR;

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        LOGD("Compressed GL texture: 0x%04x, %ix%i, %i bytes, data: %p", internalFormat, width, height, size, data);

        glCompressedTexImage2D(GL_TEXTURE_2D,
            0,
            internalFormat,
            width,
            height,
            0,
            size,
            data);

        glBindTexture(GL_TEXTURE_2D,0);

        return id;
    }


    TextureResource::TextureResource(std::string const& filename):
        Resource(filename),
        hasAlpha_(false)
    {
        // We need ImageResource as source to the texture
        std::string id = "image" + filename;
        imageResource_ = dynamic_cast<ImageResource*>(ResourceManagerPrivate::resource(id));
        if (imageResource_ == nullptr) {
            imageResource_ = new ImageResource(filename);
            ResourceManagerPrivate::setResource(id, imageResource_);
        }
        imageResource_->increment();
        width_ = imageResource_->width();
        height_ = imageResource_->height();
        format_ = imageResource_->format();
        hasAlpha_ = imageResource_->format() == Bitmap::Format::RGBA_8888 ? true : false;
    }

    TextureResource::TextureResource(Bitmap& bitmap, bool mipmaps):
        Resource(""),
        imageResource_(new ImageResource(bitmap)),
        hasAlpha_(false),
        mipmaps_(mipmaps)
    {
        width_ = imageResource_->width();
        height_ = imageResource_->height();
        imageResource_->increment();
        format_ = imageResource_->format();
        hasAlpha_ = imageResource_->format() == Bitmap::Format::RGBA_8888 ? true : false;
        AnimationManager::setSceneDirty();
    }

    TextureResource::TextureResource(uint32_t EGLTextureId, bool mipmaps, bool managed, bool external, bool compressed):
        Resource(""),
        textureId_(EGLTextureId),
        mipmaps_(mipmaps),
        external_(external),
        managed_(managed),
        compressed_(compressed)
    {
        AnimationManager::setSceneDirty();
    }

    TextureResource::TextureResource(uint32_t internalFormat,
        uint32_t width, uint32_t height, uint32_t size,
        const uint8_t *data, bool compressed, bool mipmaps):
        Resource(""),
        generalImageResource_(new GeneralImageResource(internalFormat, width, height, size, data, compressed)),
        mipmaps_(mipmaps),
        compressed_(compressed)
    {
        width_ = generalImageResource_->width();
        height_ = generalImageResource_->height();

        if(compressed_){
            uvScale_.y(-1.f);
            uvOffset_.y(1.f);
        }
        AnimationManager::setSceneDirty();
        format_ = Bitmap::Format::RGBA_8888;
    }

    TextureResource::~TextureResource()
    {
        if(managed_)
        {
            if (textureId_ != 0) {
                staleTextureIds_.push_back(textureId_);
            }
            if (textureId2_ != 0) {
                staleTextureIds_.push_back(textureId2_);
            }

            if (fboId_ != 0) {
                staleFramebufferIds_.push_back(fboId_);
                staleRenderbufferIds_.push_back(depthBuffer_);
            }

            if (imageResource_ != nullptr) {
                imageResource_->decrement();
                imageResource_ = nullptr;
            }

            if (generalImageResource_ != nullptr) {
                generalImageResource_->decrement();
                generalImageResource_ = nullptr;
            }
        }
    }

    unsigned int TextureResource::id()
    {
        if(textureId_ == 0)
        {
            initialize();
        }
        return textureId_;
    }

    unsigned int TextureResource::id2()
    {
        if(textureId_ == 0) // textureId_ defines also is the id2 defined.
        {
            initialize();
        }
        return textureId2_;
    }

    GLenum TextureResource::glFormat(unsigned int internalFormat)
    {
        switch(internalFormat) {
            case Bitmap::Format::LUMINANCE_8:
            case Bitmap::Format::YUV420SP_NV12:
            case Bitmap::Format::YUV420SP_NV21:
            {
                return GL_LUMINANCE;
            }
            case Bitmap::Format::RGBA_8888:
            {
                return GL_RGBA;
            }
            case Bitmap::Format::RGB_888:
            {
                return GL_RGB;
            }
            default:
            {
                LOGE("Format is not supported. Using GL_RGBA, results are unknown!");
                return GL_RGBA;
            }
        }
    }

    void TextureResource::initialize()
    {
        if(imageResource_ || (generalImageResource_ && !generalImageResource_->compressed()))
        {
            unsigned int internalFormat;
            int width;
            int height;
            char *pixels;

            if(imageResource_){
                internalFormat = imageResource_->format();
                width = imageResource_->width();
                height = imageResource_->height();
                pixels = imageResource_->pixels();
            }else{
                internalFormat = generalImageResource_->format();
                width = generalImageResource_->width();
                height = generalImageResource_->height();
                pixels = (char*)generalImageResource_->data();
            }

            GLenum format = glFormat(internalFormat);

            // Texture 1
            if(Bitmap::Format::LUMINANCE_8 == internalFormat ||
                Bitmap::Format::RGBA_8888 == internalFormat ||
                Bitmap::Format::RGB_888 == internalFormat ||
                Bitmap::Format::YUV420SP_NV12 == internalFormat ||
                Bitmap::Format::YUV420SP_NV21 == internalFormat) {
                textureId_ = createTexture(mipmaps_, width, height, format, pixels);
            }

            // Texture 2
            // Currently, the second texture is containing YUV's UV data
            if(Bitmap::Format::YUV420SP_NV12 == internalFormat ||
                Bitmap::Format::YUV420SP_NV21 == internalFormat) {
                unsigned int offset = width * height;
                textureId2_ = createTexture(mipmaps_, (width + 1) / 2, (height + 1) / 2, GL_LUMINANCE_ALPHA, pixels + offset);
            }

            if(imageResource_){
                imageResource_->decrement();
                imageResource_ = nullptr;
            }else{
                generalImageResource_->decrement();
                generalImageResource_ = nullptr;
            }
        } else if(generalImageResource_ && generalImageResource_->compressed()) {
            textureId_ = createCompressedTexture(generalImageResource_->format(), generalImageResource_->width(), generalImageResource_->height(), generalImageResource_->size(), generalImageResource_->data());
            generalImageResource_->decrement();
            generalImageResource_ = nullptr;
        }
    }

    bool TextureResource::hasAlpha()
    {
        return hasAlpha_;
    }

    Grapevine::Bitmap::Format::Enum TextureResource::format()
    {
        return format_;
    }

    bool TextureResource::isExternal() const
    {
        return external_;
    }

    Vector2 TextureResource::uvScale() const
    {
        return uvScale_;
    }

    Vector2 TextureResource::uvOffset() const
    {
        return uvOffset_;
    }

    float TextureResource::uvRotation() const
    {
        return rotation_;
    }

    void TextureResource::setUVScale(Vector2 scale)
    {
        uvScale_ = scale;
    }

    void TextureResource::setUVOffset(Vector2 offset)
    {
        uvOffset_ = offset;
    }

    void TextureResource::setUVRotation(float rotation)
    {
        rotation_ = rotation;
    }

    void TextureResource::setInternalFormat(Bitmap::Format::Enum format)
    {
        format_ = format;
    }

    void TextureResource::texSubImage( GLint level,
        GLint xoffset,GLint yoffset,GLint width,GLint height,
        GLenum format, GLsizei imageSize, GLvoid *data)
    {
        GLenum target = external_ ? GL_TEXTURE_EXTERNAL_OES : GL_TEXTURE_2D;

        glBindTexture(target,textureId_);

        if(compressed_){
            glCompressedTexSubImage2D(target,level,xoffset,yoffset,width,height,format,imageSize,data);
            // LOGI("TextureAtlasFactory: %i glCompressedTexSubImage2D format 0x%04x, : %i %i %i %i, size %i error 0x%x",textureId_,format,xoffset,yoffset,width,height,imageSize,glGetError());
        }else{
            glTexSubImage2D(target,level,xoffset,yoffset,width,height,format,GL_UNSIGNED_BYTE,data);
            // LOGI("TextureAtlasFactory: %i glTexSubImage2D format 0x%04x, : %i %i %i %i error 0x%x",textureId_,format,xoffset,yoffset,width,height,glGetError());
        }

        glBindTexture(target, 0);
    }

    void TextureResource::bindAsFramebuffer()
    {
        bool initialized = fboId_;
        if (!initialized) {
            glGenFramebuffers(1, &fboId_);
            glGenRenderbuffers(1, &depthBuffer_);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, fboId_);

        if (!initialized) {
            glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer_);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width_, height_);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, id(), 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer_);
        }
    }

    void TextureResource::unbindFramebuffer()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void TextureResource::purgeStaleTextures()
    {
        if (!staleTextureIds_.empty()) {
            glDeleteTextures(staleTextureIds_.size(), &staleTextureIds_[0]);
            staleTextureIds_.clear();
        }

        if (!staleFramebufferIds_.empty()) {
            glDeleteFramebuffers(staleFramebufferIds_.size(), &staleFramebufferIds_[0]);
            staleFramebufferIds_.clear();
        }

        if (!staleRenderbufferIds_.empty()) {
            glDeleteRenderbuffers(staleRenderbufferIds_.size(), &staleRenderbufferIds_[0]);
            staleRenderbufferIds_.clear();
        }
    }

    std::vector<unsigned int> TextureResource::staleTextureIds_;
    std::vector<unsigned int> TextureResource::staleFramebufferIds_;
    std::vector<unsigned int> TextureResource::staleRenderbufferIds_;
}
