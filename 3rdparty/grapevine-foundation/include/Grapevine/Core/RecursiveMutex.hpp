/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CORE_RECURSIVEMUTEX
#define GRAPEVINE_CORE_RECURSIVEMUTEX

#include <pthread.h>
#include "Grapevine/Core/Class.hpp"

#if defined(PTHREAD_MUTEX_RECURSIVE_NP)
#define GRAPEVINE_MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE_NP
#else
#define GRAPEVINE_MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE
#endif

namespace Grapevine
{
    /**
     * @class RecursiveMutex
     *
     * RecursiveMutex a mutex class for multithreaded applications.
     */
    class RecursiveMutex
    {
    public:
        CLASS_UNCOPYABLE(RecursiveMutex);

        RecursiveMutex();
        ~RecursiveMutex();
        int lock();
        int tryLock();
        int unlock();

    private:
        friend class Condition;
        pthread_mutex_t mutex_;
    };
}

#endif
