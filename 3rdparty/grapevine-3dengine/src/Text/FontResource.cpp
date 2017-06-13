/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "FontResource.hpp"
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Core/Exception.hpp>
#include <string>

namespace Grapevine
{
    FontResource::FontResource(std::string const& filename):
        Resource(filename),
        boundingSphereRadius_(0.0f)
    {
        font_ = new Font(path());
    }

    FontResource::~FontResource()
    {
        delete font_;
    }

    Font *FontResource::font()
    {
        return font_;
    }

    float FontResource::getScale(float fontSize) const
    {
        return font_->get_scale(fontSize);
    }

    int FontResource::getKerning(int left, int right) const
    {
        return font_->get_kerning(left, right);
    }

    int FontResource::getAdvanceWidth(int code) const
    {
        return font_->get_glyph_metrics(code).advanceWidth;
    }
}

