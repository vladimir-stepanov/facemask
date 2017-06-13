/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CORE_RECURSIVELOCK
#define GRAPEVINE_CORE_RECURSIVELOCK

#include "RecursiveMutex.hpp"
#include "Grapevine/Core/Class.hpp"

#define RECURSIVE_LOCK Grapevine::RecursiveLock lock(mutex_);

namespace Grapevine
{
    /**
     * @class RecursiveLock
     *
     * Lock a class to help usage of mutex.
     */
    class RecursiveLock
    {
    public:
        CLASS_UNCOPYABLE(RecursiveLock);

        RecursiveLock(RecursiveMutex& mutex):
            mutex_(&mutex)
        {
            mutex_->lock();
        }

        ~RecursiveLock()
        {
            mutex_->unlock();
        }

    private:
        RecursiveMutex* mutex_;
    };
}

#endif
