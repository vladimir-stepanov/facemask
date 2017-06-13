/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CORE_PROCESS
#define GRAPEVINE_CORE_PROCESS

#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include "Grapevine/Core/Class.hpp"

namespace Grapevine
{
    /**
     * @class Process
     *
     * Process is a helper class.
     */
    class Process
    {
    public:
        CLASS_UNCOPYABLE(Process);

        static unsigned int currentId()
        {
            return getpid();
        }

    private:
        Process();
        ~Process();
    };
}

#endif
