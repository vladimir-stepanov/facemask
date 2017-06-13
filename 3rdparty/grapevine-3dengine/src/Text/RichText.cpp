/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <Grapevine/Text/RichText.hpp>

namespace Grapevine
{
    RichText::RichText()
    {
    }

    RichText::RichText(Text const& text)
    {
        addText(text);
    }

    RichText::RichText(RichText const& r)
    {
        if (this != &r)
        {
            textEntries_ = r.textEntries_;
        }
    }

    RichText::~RichText()
    {
    }

    void RichText::addText(Text const& text)
    {
        textEntries_.push_back(text);
        // TODO implement private and change signaling!
    }

    size_t RichText::size() const
    {
        return textEntries_.size();
    }

    Text const& RichText::text(size_t index) const
    {
        return textEntries_[index];
    }

    RichText& RichText::operator=(RichText const& r)
    {
        if (this != &r)
        {
            textEntries_ = r.textEntries_;
        }
        return *this;
    }
}
