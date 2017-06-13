/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_TEXT_FONTRESOURCE
#define GRAPEVINE_TEXT_FONTRESOURCE

#include "Grapevine/Resource/Resource.hpp"
#include "../kaski/font.hpp"

namespace Grapevine
{
    class FontResource: public Resource
    {
    public:
        FontResource(std::string const& filename);
        ~FontResource();
        Font *font();
        float getScale(float fontSize) const;
        int getKerning(int left, int right) const;
        int getAdvanceWidth(int code) const;

        bool operator==(const FontResource& r) const
        {
            return path() == r.path();
        }

        int getGlyphCount() const;
        const math::vec2 &getGlyphPosition(int index) const;

    private:
        Font *font_;
        float boundingSphereRadius_;
    };
}

#endif
