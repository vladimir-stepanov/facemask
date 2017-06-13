/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATERIAL_TEXTURERESOURCE
#define GRAPEVINE_MATERIAL_TEXTURERESOURCE

#include <string>
#include <vector>
#include "Grapevine/Resource/Resource.hpp"
#include <Grapevine/Image/Bitmap.hpp>
#include <Grapevine/Math/Vector.hpp>
#include <Grapevine/GPU/GL.hpp>

namespace Grapevine
{
    class ImageResource;
    class GeneralImageResource;

    class TextureResource: public Resource
    {
    public:
        TextureResource(std::string const& filename);
        TextureResource(Bitmap& filename, bool mipmaps = true);
        TextureResource(uint32_t GLTextureId, bool mipmaps = true, bool managed = true, bool external = true, bool compressed = false);
        TextureResource(uint32_t internalFormat,
            uint32_t width, uint32_t height, uint32_t size,
            const uint8_t *data, bool compressed = false, bool mipmaps = false);
        ~TextureResource();

        /**
         * The id and id2 are the GPU texture ids. id() is defined
         * for all of the texture formats. It is the only id for formats
         * excluding the YUV ones currently. With YUV formats next
         * differences apply:
         *
         * YUV420sp_NV12: id() is the Y data and id2() is the UV data in GL_LUMINANCE_ALPHA format.
         * YUV420sp_NV21: id2() is the Y data and id() is the UV data in GL_LUMINANCE_ALPHA format.
         */
        unsigned int id();

        /**
         * look id();
         */
        unsigned int id2();

        bool hasAlpha();
        Grapevine::Bitmap::Format::Enum format();
        bool isExternal() const;

        void setUVScale(Vector2 scale); // uvScale
        void setUVOffset(Vector2 offset); // uvOffset
        void setUVRotation(float radians);
        void setInternalFormat(Bitmap::Format::Enum format);

        Vector2 uvScale() const;
        Vector2 uvOffset() const;
        float uvRotation() const;

        void texSubImage(GLint level,GLint xoffset,GLint yoffset,GLint width,GLint height, GLenum format, GLsizei imageSize, GLvoid *data);

        void bindAsFramebuffer();
        void unbindFramebuffer();

        static void purgeStaleTextures();
    private:
        static std::vector<unsigned int> staleTextureIds_;
        static std::vector<unsigned int> staleFramebufferIds_;
        static std::vector<unsigned int> staleRenderbufferIds_;

        GLenum glFormat(unsigned int internalFormat);

        ImageResource* imageResource_ = nullptr;
        GeneralImageResource *generalImageResource_ = nullptr;
        void initialize();
        unsigned int textureId_ = 0;
        unsigned int textureId2_ = 0;

        unsigned int fboId_ = 0;
        unsigned int depthBuffer_ = 0;

        unsigned int width_ = 0;
        unsigned int height_ = 0;

        bool hasAlpha_ = true;
        bool mipmaps_ = true;
        Bitmap::Format::Enum format_;
        bool external_ = false;
        Vector2 uvScale_ = Vector2(1.f);
        Vector2 uvOffset_ = Vector2(0.f);
        float rotation_ = 0.f;
        bool managed_ = true;
        bool compressed_ = false;

        friend class TextureFactory;
    };
}

#endif
