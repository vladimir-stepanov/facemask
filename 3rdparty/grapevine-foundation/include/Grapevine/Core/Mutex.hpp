/**
 * Copyright (C) 2013-2015 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CORE_MUTEX_HPP
#define GRAPEVINE_CORE_MUTEX_HPP

#include <pthread.h>
#include "Grapevine/Core/Class.hpp"

namespace Grapevine
{
    /**
     * @class Mutex
     *
     * NOTE: please use the .h version as your include in Atelier OS project! This header will be deprecated!
     *
     * Mutex a mutex class for multithreaded applications.
     */
    class Mutex
    {
    public:
        CLASS_UNCOPYABLE(Mutex);

        Mutex();
        ~Mutex();
        int lock();
        int tryLock();
        int unlock();

    private:
        friend class Condition;
        pthread_mutex_t mutex_;
    };
}

#endif

