/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_TEXT_RICHTEXT
#define GRAPEVINE_TEXT_RICHTEXT

#include <Grapevine/Text/Text.hpp>
#include <Grapevine/Core/Class.hpp>
#include <vector>

namespace Grapevine
{
    class RichText
    {
    public:
        CLASS_COPYABLE(RichText)

        RichText();
        RichText(Text const& text);
        ~RichText();
        void addText(Text const& text);
        size_t size() const;
        Text const& text(size_t index) const;

    private:
        std::vector<Text> textEntries_;
    };
}

#endif
