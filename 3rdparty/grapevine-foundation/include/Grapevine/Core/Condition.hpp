/**
 * Copyright (C) 2013-2015 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CORE_CONDITION_HPP
#define GRAPEVINE_CORE_CONDITION_HPP

#include "Grapevine/Core/Class.hpp"
#include "Grapevine/Core/Mutex.hpp"
#include <pthread.h>
#include <sys/time.h>

namespace Grapevine
{
    /**
     * @class Condition
     *
     * NOTE: please use the .h version as your include in Atelier OS project! This header will be deprecated!
     *
     * Condition class for multithreaded applications.
     */
    class Condition
    {
    public:
        CLASS_UNCOPYABLE(Condition);

        Condition();
        ~Condition();
        int wait(Mutex& mutex);
        int timedWait(Mutex& mutex, int seconds);
        int timedWait(Mutex& mutex, struct timespec timeouttime);
        void signal();
        void broadcast();

    private:
        void init();
        pthread_cond_t condition_;
        bool okCreation_;
    };
}

#endif

