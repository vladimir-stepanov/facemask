/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATERIAL_CUBEMAPRESOURCE
#define GRAPEVINE_MATERIAL_CUBEMAPRESOURCE

#include <string>
#include <vector>
#include "Grapevine/Resource/Resource.hpp"
#include <Grapevine/Image/Bitmap.hpp>
#include "Grapevine/Material/CubemapAsset.hpp"

namespace Grapevine
{
    class ImageResource;

    class CubemapResource: public Resource
    {
    public:
        CubemapResource(std::string const& filename, CubemapAsset::Type::Enum type);
        CubemapResource(Bitmap& filename, CubemapAsset::Type::Enum type);
        ~CubemapResource();
        unsigned int textureId();
        static void purgeStaleCubemaps();
    private:

        static std::vector<unsigned int> staleTextureIds_;
        ImageResource* imageResource_;
        void initialize();
        unsigned int textureId_;
        CubemapAsset::Type::Enum type_;
        friend class TextureFactory;
    };
}

#endif
