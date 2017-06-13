/**
 * Copyright (C) 2013-2016 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SHADER_FRAGMENTSOURCE
#define GRAPEVINE_SHADER_FRAGMENTSOURCE

#include "ShaderSource.hpp"

namespace Grapevine
{
    /**
     * Fragment shader source class.
     */
    class FragmentSource: public ShaderSource
    {
    public:
        /**
         * Constructor with empty source.
         */
        FragmentSource();

        /**
         * Constructor with source loaded from given filename.
         */
        FragmentSource(std::string const& filename);

        /**
         * Constructor with source loaded from given filename.
         */
        FragmentSource(char const* filename);
   };
}

#endif
