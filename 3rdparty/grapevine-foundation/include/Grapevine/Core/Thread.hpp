/**
 * Copyright (C) 2013-2015 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CORE_THREAD_HPP
#define GRAPEVINE_CORE_THREAD_HPP

#include "Grapevine/Core/Class.hpp"
#include "Grapevine/Core/Lock.hpp"
#include <pthread.h>
#include <string>

namespace Grapevine
{
    struct ThreadCToCppWrapper;

    /**
     * @class Thread
     *
     * NOTE: please use the .h version as your include in Atelier OS project! This header will be deprecated!
     *
     * Thread is a base class for thread classes.
     */
    class Thread
    {
    public:
        CLASS_UNCOPYABLE(Thread)

        void start();
        void join();
        void cancel();

    protected:
        virtual void run() = 0;
        Thread();
        virtual ~Thread();

    private:
        pthread_t thread_;
        friend struct ThreadCToCppWrapper;
        Mutex mtx_;
        volatile bool running_;
    };

    struct ThreadCToCppWrapper
    {
        static void protectedCppRun(Thread* other)
        {
            Lock lock(other->mtx_);
            other->running_ = true;
            other->run();
        }
    };
}

#endif
