/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATERIAL_TEXTUREASSET
#define GRAPEVINE_MATERIAL_TEXTUREASSET

#include <string>
#include <Grapevine/Image/Bitmap.hpp>
#include <Grapevine/Math/Vector.hpp>
#include <Grapevine/Core/Class.hpp>

namespace Grapevine
{
    class TextureResource;

    /**
     * @class TextureAsset
     *
     * TextureAsset is a class to be used for textures.
     */
    class TextureAsset
    {
    public:
        CLASS_COPYABLE(TextureAsset);

        TextureAsset(char const* filename);
        TextureAsset(std::string const& filename);
        TextureAsset(uint32_t interalFormat, uint32_t width, uint32_t height, uint32_t size, const uint8_t *data, bool compressed = false, bool mipmaps = true);
        TextureAsset(Bitmap& bitmap, bool mipmaps = true);
        TextureAsset(uint32_t EGLTextureId, bool mipmaps = true, bool managed = true, bool external = true, bool compressed = true);
        TextureAsset();
        TextureAsset(TextureResource* resource);
        ~TextureAsset();

        /**
         * Force intialize GPU data. GPU data initialization is
         * handled automatically by drawing normally.
         *
         * @note Use this only when in need of syncronous loading in
         * thread with GL context.
         */
        void initializeGPUData();
        TextureResource* resource();

    protected:
        TextureResource* resource_;
    };
}

#endif
