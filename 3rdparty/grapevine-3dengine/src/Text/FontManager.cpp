/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "FontManagerPrivate.hpp"
#include "Grapevine/Text/FontManager.hpp"
#include <Grapevine/Core/Exception.hpp>
#include <Grapevine/Core/Log.hpp>

namespace Grapevine
{
    FontManagerPrivate* FontManager::private_ = NULL;

    FontManager::FontManager()
    {
        if (private_ == NULL)
        {
            LOG
            private_ = new FontManagerPrivate();
        }
        else
        {
            throw Exception("FontManager exists already! Only one instance is needed.");
        }
    }

    FontManager::~FontManager()
    {
        if (private_ != NULL)
        {
            delete private_;
        }
        else
        {
            LOGD("no FontManager used.");
        }
    }

    std::vector<std::string> FontManager::fontFiles()
    {
        if (private_ != NULL)
        {
            return private_->fontFiles();
        }
        else
        {
            throw Exception("fontFiles() can be called only when FontManager exists!");
        }
    }
}
