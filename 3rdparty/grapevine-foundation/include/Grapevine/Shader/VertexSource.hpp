/**
 * Copyright (C) 2013-2016 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SHADER_VERTEXSOURCE
#define GRAPEVINE_SHADER_VERTEXSOURCE

#include "ShaderSource.hpp"
#include <Grapevine/Core/String.hpp>

namespace Grapevine
{
    /**
     * Vertex shader source class.
     */
    class VertexSource: public ShaderSource
    {
    public:
        /**
         * Constructor with empty source.
         */
        VertexSource();

        /**
         * Constructor with source loaded from given filename.
         */
        VertexSource(std::string const& filename);

        /**
         * Constructor with source loaded from given filename.
         */
        VertexSource(char const* filename);
    };
}

#endif
