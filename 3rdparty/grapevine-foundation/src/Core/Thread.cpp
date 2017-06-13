/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Core/Thread.hpp"
#include "Grapevine/Core/Log.hpp"
#include "Grapevine/Core/Exception.hpp"
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <stdexcept>

namespace Grapevine
{
    void* thread_function(void* instance)
    {
        try
        {
            Thread* tmp = (Thread*)instance;
            ThreadCToCppWrapper::protectedCppRun(tmp);
        }
        catch (Exception const& e)
        {
            LOGE("Exception inside thread!\n%s", e.what());
        }
        catch (std::exception const &e)
        {
            LOGE("Exception inside thread!\n%s", e.what());
        }

        return NULL;
    }

    Thread::Thread():
        running_(false)
    {
    }

    Thread::~Thread()
    {
    }

    void Thread::start()
    {
        pthread_attr_t type;
        if (pthread_attr_init(&type) != 0)
        {
            throw Exception("pthread_attr_init failed");
        }
        pthread_attr_setdetachstate(&type, PTHREAD_CREATE_JOINABLE);

        int result = pthread_create(&thread_, &type, thread_function, (void*)this);
        if (result == EAGAIN)
        {
            throw Exception("Thread::start, errno:EAGAIN");
        }
        else if (result == EINVAL)
        {
            throw Exception("Thread::start, errno:EINVAL");
        }
        else if (result == EPERM)
        {
            throw Exception("Thread::start, errno:EPERM");
        }

        while(!running_)
        {
            usleep(1);
        }
    }

    void Thread::join()
    {
        /*
        int result = pthread_join(thread_, NULL);
        if (result == EINVAL)
        {
            throw Exception("Thread::join, errno:EINVAL");
        }
        else if (result == ESRCH)
        {
            throw Exception("Thread::join, errno:ESRCH");
        }
        else if (result == EDEADLK)
        {
            throw Exception("Thread::join, errno:EDEADLK");
        }
        */

        Lock lock(mtx_);
    }

    void Thread::cancel()
    {
#ifndef ANDROID
        int result = pthread_cancel(thread_);
        if (result == ESRCH)
        {
            throw Exception("Thread::cancel, errno::ESRCH");
        }
#else
            throw Exception("Thread::cancel, not support on Android");
#endif
    }
}
