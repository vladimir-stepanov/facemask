/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Resource/Resource.hpp"
#include <string>

namespace Grapevine
{
    Resource::Resource(std::string const& filename):
        Referenced(),
        filename_(filename)
    {
    }

    Resource::~Resource()
    {
    }

    std::string const& Resource::path() const
    {
        return filename_;
    }
}
