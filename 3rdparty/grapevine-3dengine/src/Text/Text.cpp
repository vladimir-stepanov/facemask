/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <iconv.h>
#include <stdint.h>
#include <Grapevine/Text/Text.hpp>

namespace Grapevine
{
    Text::Text(std::string const& text, FontAsset const& fontAsset, unsigned int size, Vector4 const& color):
        fontAsset_(fontAsset),
        size_(size),
        color_(color)
    {
        static iconv_t encoder = (iconv_t)-1;
        if (encoder == (iconv_t)-1)
        {
            // create encoder when needed
            encoder = iconv_open("UTF-16LE//TRANSLIT", "UTF-8");
        }

        if (!text.empty() && encoder != (iconv_t)-1)
        {
            static size_t maxSize = 0;
            static char* tempText = NULL;
            static uint16_t* unicode = NULL;

            // increase temporary storage if needed
            if (maxSize < text.size() + 1)
            {
                // remove old storage
                delete[] unicode;
                delete[] tempText;

                // allocate new storage
                maxSize = text.size() + 1;
                tempText = new char[maxSize];
                unicode = new uint16_t[maxSize * sizeof(uint16_t)];
            }

            // copy input string to temporary storage
            text.copy(tempText, text.size());
            tempText[text.size()] = 0;

            // store original unicode array location
            char* outcodes = reinterpret_cast<char*>(unicode);

            // setup in & out code pointers
            size_t inBytes = text.size() + 1;
            size_t outBytes = inBytes * sizeof(uint16_t);
            char* incodePtr = tempText;
            char* outcodePtr = outcodes;

            iconv(encoder, &incodePtr, &inBytes, &outcodePtr, &outBytes);

            // copy converted characters to result string
            int outputCharacters = (outcodePtr - outcodes) / sizeof(uint16_t);
            for (int i=0; i<outputCharacters; ++i)
            {
                text_ += unicode[i];
            }
        }
    }

    Text::Text(std::wstring const& text, FontAsset const& fontAsset, unsigned int size, Vector4 const& color):
        text_(text),
        fontAsset_(fontAsset),
        size_(size),
        color_(color)
    {
    }

    Text::Text(Text const& r):
        text_(r.text_),
        fontAsset_(r.fontAsset_),
        size_(r.size_),
        color_(r.color_)
    {
    }

    Text::~Text()
    {
    }

    std::wstring const& Text::text() const
    {
        return text_;
    }

    FontAsset const& Text::fontAsset() const
    {
        return fontAsset_;
    }

    unsigned int Text::size() const
    {
        return size_;
    }

    Vector4 const& Text::color() const
    {
        return color_;
    }

    Text& Text::operator=(Text const& r)
    {
        if (this != &r)
        {
            text_ = r.text_;
            fontAsset_ = r.fontAsset_;
            size_ = r.size_;
            color_ = r.color_;
        }
        return *this;
    }
}
