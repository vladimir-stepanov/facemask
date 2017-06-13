/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_TEXT_FONTMANAGER
#define GRAPEVINE_TEXT_FONTMANAGER

#include <Grapevine/Core/Class.hpp>

namespace Grapevine
{
    class FontManagerPrivate;

    class FontManager
    {
    public:
        CLASS_UNCOPYABLE(FontManager)

        FontManager();
        ~FontManager();

        /**
         * @returns: available font files (files with .ttf suffix)
         */
        static std::vector<std::string> fontFiles();

    private:
        static FontManagerPrivate* private_;
    };
}

#endif
