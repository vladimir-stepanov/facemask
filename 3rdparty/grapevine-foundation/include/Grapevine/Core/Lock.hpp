/**
 * Copyright (C) 2013-2015 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CORE_LOCK_HPP
#define GRAPEVINE_CORE_LOCK_HPP

#include "Mutex.hpp"
#include "Grapevine/Core/Class.hpp"

#define LOCK Grapevine::Lock lock(mutex_);

namespace Grapevine
{
    /**
     * @class Lock
     *
     * NOTE: please use the .h version as your include in Atelier OS project! This header will be deprecated!
     *
     * Lock a class to help usage of mutex.
     */
    class Lock
    {
    public:
        CLASS_UNCOPYABLE(Lock);

        Lock(Mutex& mutex):
            mutex_(&mutex)
        {
            mutex_->lock();
        }

        ~Lock()
        {
            mutex_->unlock();
        }

    private:
        Mutex* mutex_;
    };

    class Unlock
    {
    public:
        CLASS_UNCOPYABLE(Unlock);

        Unlock(Mutex &mutex):
            mutex_(&mutex)
        {
            mutex_->unlock();
        }

        ~Unlock()
        {
            mutex_->lock();
        }

    private:
        Mutex* mutex_;
    };
}

#endif
