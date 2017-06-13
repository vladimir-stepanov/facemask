/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "FontManagerPrivate.hpp"
#include <Grapevine/Core/Exception.hpp>
#include "../Resource/ResourceManagerPrivate.hpp"

namespace Grapevine
{
    FontManagerPrivate* FontManagerPrivate::singleton_ = NULL;

    FontManagerPrivate::FontManagerPrivate()
    {
        singleton_ = this;
    }

    FontManagerPrivate::~FontManagerPrivate()
    {
    }

    std::vector<std::string> FontManagerPrivate::fontFiles()
    {
        return ResourceManagerPrivate::filesWithSuffix(".ttf");
    }
}
