/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_TEXT_FONTMANAGERPRIVATE
#define GRAPEVINE_TEXT_FONTMANAGERPRIVATE

#include <string>
#include <vector>

namespace Grapevine
{
    class FontManagerPrivate;

    class FontManagerPrivate
    {
    public:
        std::vector<std::string> fontFiles();

    private:
        static FontManagerPrivate* singleton_;

        friend class FontManager;
        FontManagerPrivate();
        ~FontManagerPrivate();
    };
}

#endif
