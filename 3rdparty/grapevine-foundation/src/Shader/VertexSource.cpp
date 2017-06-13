/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Shader/VertexSource.hpp"

namespace Grapevine
{
    VertexSource::VertexSource():
        ShaderSource(ShaderType::Vertex)
    {
    }

    VertexSource::VertexSource(std::string const& filename):
        ShaderSource(filename, ShaderType::Vertex)
    {
    }

    VertexSource::VertexSource(char const* filename):
        ShaderSource(std::string(filename), ShaderType::Vertex)
    {
    }
}
