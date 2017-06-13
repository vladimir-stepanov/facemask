/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATERIAL_CUBEMAPASSET
#define GRAPEVINE_MATERIAL_CUBEMAPASSET

#include <string>
#include <Grapevine/Image/Bitmap.hpp>
#include <Grapevine/Core/ReferencedPointer.hpp>

namespace Grapevine
{
    class CubemapResource;

    /**
     * @class CubemapAsset
     *
     * CubemapAsset is a class to be used in cubemap reflections.
     *
     * @note Strip format is currently only supported.
     */
    class CubemapAsset
    {
    public:
        CLASS_COPYABLE(CubemapAsset);

        struct Type
        {
            enum Enum
            {
                HorizontalCross,
                Strip // order: front, left, back, right, top, bottom
            };
        };

        CubemapAsset(char const* filename, Type::Enum type = Type::Strip);
        CubemapAsset(std::string const& filename, Type::Enum type = Type::Strip);
        CubemapAsset(Bitmap& bitmap, Type::Enum type = Type::Strip);
        ~CubemapAsset();
        CubemapResource* resource();

    private:
        ReferencedPointer<CubemapResource> resource_;
    };
}

#endif
