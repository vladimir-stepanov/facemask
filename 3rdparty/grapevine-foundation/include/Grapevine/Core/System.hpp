/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CORE_SYSTEM
#define GRAPEVINE_CORE_SYSTEM

#include "Grapevine/Core/Class.hpp"
#include "Grapevine/Core/Log.hpp"
#include "Grapevine/Core/String.hpp"
#include <stdlib.h>

namespace Grapevine
{
    /**
     * @class System
     *
     * System is a helper class.
     */
    namespace System
    {
        String home()
        {
            String r;
            char* tmp = NULL;

            tmp = getenv("HOME");
            if (tmp == NULL)
            {
                Log::E("System: $HOME has not been set, returning NULL.\n");
            }

            r = String(tmp);

            return r;
        }
    }
}

#endif
