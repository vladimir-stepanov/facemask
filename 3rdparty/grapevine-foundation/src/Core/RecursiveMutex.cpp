/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Core/RecursiveMutex.hpp"
#include "Grapevine/Core/Log.hpp"
#include "Grapevine/Core/Exception.hpp"
#include <stdlib.h>
#include <errno.h>

namespace Grapevine
{
    RecursiveMutex::RecursiveMutex()
    {
        pthread_mutexattr_t mutexAttributes;
        pthread_mutexattr_settype(&mutexAttributes, GRAPEVINE_MUTEX_RECURSIVE);
        pthread_mutexattr_init(&mutexAttributes);

        int r = pthread_mutex_init(&mutex_, &mutexAttributes);
        if (r == EINVAL)
        {
            throw Exception("RecursiveMutex(), errno:EAGAIN");
        }
        else if (r == ENOMEM)
        {
            throw Exception("RecursiveMutex(), errno:ENOMEM");
        }
        else if (r == EPERM)
        {
            throw Exception("RecursiveMutex(), errno:EPERM");
        }

        r = pthread_mutexattr_destroy(&mutexAttributes);
        if (r != 0)
        {
            throw Exception("RecursiveMutex(), pthread_mutexattr_destroy failed");
        }
    }

    RecursiveMutex::~RecursiveMutex()
    {
        int r = 0;

        r = pthread_mutex_destroy(&mutex_);
        if (r == EINVAL)
        {
            throw Exception("~RecursiveMutex, errno:EINVAL");
        }
        else if (r == EBUSY)
        {
            throw Exception("~RecursiveMutex, errno:EBUSY");
        }
    }

    int RecursiveMutex::lock()
    {
        return pthread_mutex_lock(&mutex_);
    }

    int RecursiveMutex::tryLock()
    {
        return pthread_mutex_trylock(&mutex_);
    }

    int RecursiveMutex::unlock()
    {
        return pthread_mutex_unlock(&mutex_);
    }
}
