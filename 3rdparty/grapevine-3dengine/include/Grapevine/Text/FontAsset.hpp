/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_TEXT_FONTASSET
#define GRAPEVINE_TEXT_FONTASSET

#include <string>
#include <Grapevine/Core/Class.hpp>

namespace Grapevine
{
    class FontResource;

    class FontAsset
    {
    public:
        CLASS_COPYABLE(FontAsset)

        FontAsset(std::string const& filename, float scale = 1.0f);
        ~FontAsset();
        FontResource* resource() const;
        std::string const& name() const;
        float getScale(float fontSize) const;
        int getKerning(int left, int right) const;
        int getAdvanceWidth(int code) const;
        bool operator==(const FontAsset& r) const;

    private:
        FontResource* resource_;
    };
}

#endif
