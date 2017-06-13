/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Shader/FragmentSource.hpp"

namespace Grapevine
{
    FragmentSource::FragmentSource():
        ShaderSource(ShaderType::Fragment)
    {
    }

    FragmentSource::FragmentSource(std::string const& filename):
        ShaderSource(filename, ShaderType::Fragment)
    {
    }

    FragmentSource::FragmentSource(char const* filename):
        ShaderSource(std::string(filename), ShaderType::Fragment)
    {
    }
}
