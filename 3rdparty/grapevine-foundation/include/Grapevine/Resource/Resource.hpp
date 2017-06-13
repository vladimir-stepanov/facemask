/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_RESOURCE_RESOURCE
#define GRAPEVINE_RESOURCE_RESOURCE

#include <Grapevine/Core/Referenced.hpp>
#include <string>

namespace Grapevine
{
    class Resource;

    class Resource: public Referenced
    {
    public:
        Resource(std::string const& filename);
        virtual ~Resource();
        std::string const& path() const;
        Resource* resource();

    private:
        std::string filename_;
    };
}

#endif
