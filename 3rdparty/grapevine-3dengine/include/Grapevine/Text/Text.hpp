/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_TEXT_TEXT
#define GRAPEVINE_TEXT_TEXT

#include <Grapevine/Math/Vector.hpp>
#include <Grapevine/Text/FontAsset.hpp>
#include <Grapevine/Core/Class.hpp>
#include <string>

namespace Grapevine
{
    class Text
    {
    public:
        CLASS_COPYABLE(Text)

        Text(std::string const& text, FontAsset const& fontAsset, unsigned int size, Vector4 const& color);
        Text(std::wstring const& text, FontAsset const& fontAsset, unsigned int size, Vector4 const& color);
        ~Text();
        std::wstring const& text() const;
        FontAsset const&  fontAsset() const;
        unsigned int size() const;
        Vector4 const& color() const;

    private:
        std::wstring text_;
        FontAsset fontAsset_;
        unsigned int size_;
        Vector4 color_;
    };
}

#endif
