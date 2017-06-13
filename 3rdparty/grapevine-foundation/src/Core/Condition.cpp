/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Core/Condition.hpp"
#include "Grapevine/Core/Exception.hpp"
#include "Grapevine/Core/Log.hpp"
#include <errno.h>

namespace Grapevine
{
    Condition::Condition():
        okCreation_(false)
    {
        init();
    }

    Condition::~Condition()
    {
        if (okCreation_ == true)
        {
            int tmp = pthread_cond_destroy(&condition_);
            if (tmp == EBUSY)
            {
                throw Exception("~Condition, errno:EBUSY\n");
            }
            else if (tmp == ENOMEM)
            {
                throw Exception("~Condition, errno:ENOMEM\n");
            }
        }
    }

    void Condition::init()
    {
        int tmp = 0;

        tmp = pthread_cond_init(&condition_, NULL);
        if (tmp == EAGAIN)
        {
            throw Exception("Condition::init, errno:EAGAIN");
        }
        else if (tmp == ENOMEM)
        {
            throw Exception("Condition::init, errno:ENOMEM");
        }
        else if (tmp == EBUSY)
        {
            throw Exception("Condition::init, errno:EBUSY");
        }
        else if (tmp == EINVAL)
        {
            throw Exception("Condition::init, errno:EINVAL");
        }

        okCreation_ = true;
    }

    int Condition::wait(Mutex &mutex)
    {
        return pthread_cond_wait(&condition_, &(mutex.mutex_));
    }

    void Condition::signal()
    {
        if (pthread_cond_signal(&condition_) == EINVAL)
        {
            throw Exception("SharedCondition signal(), EINVAL");
        }
    }

    void Condition::broadcast()
    {
        if (pthread_cond_broadcast(&condition_) == EINVAL)
        {
            throw Exception("SharedCondition broadcast(), EINVAL");
        }
    }

    int Condition::timedWait(Mutex& mutex, int seconds)
    {
        struct timeval now;
        struct timespec timeouttime;

        gettimeofday(&now,NULL);

        timeouttime.tv_sec = now.tv_sec + seconds;
        timeouttime.tv_nsec = now.tv_usec * 1000;

        return timedWait(mutex,timeouttime);
    }

    int Condition::timedWait(Mutex& mutex, struct timespec timeouttime)
    {
        return pthread_cond_timedwait(&condition_, &(mutex.mutex_), &timeouttime);
    }
}
