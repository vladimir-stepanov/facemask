/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Core/Timer.hpp"
#include "Grapevine/Core/Exception.hpp"
#include "Grapevine/Core/Log.hpp"
#include "Grapevine/Core/Lock.hpp"
#include <time.h>
#include <unistd.h>
#include <ctime>
#include <errno.h>
#include <sys/time.h>

#ifdef __APPLE__
#include <mach/mach_time.h>
#endif

namespace Grapevine
{
    unsigned int Timer::lastId_ = 0;

    Timer::Timer():
        Thread(),
        running_(true),
        active_(false),
        id_(lastId_++),
        ts_(0)
    {
        Thread::start();
    }

    Timer::~Timer()
    {
        running_ = false;
        stop();
        Thread::join();
    }

    bool Timer::operator==(Timer const& r)
    {
        return id_ == r.id_;
    }

    void Timer::start(unsigned int milliseconds)
    {
        // Create new
        struct timeval tv;
        gettimeofday(&tv, NULL);

        Lock lock(conditionMutex_);

        ts_%=0x1000;
        ts_++;

        timeToWait_.tv_sec = tv.tv_sec + (milliseconds / 1000);
        timeToWait_.tv_nsec = (tv.tv_usec * 1000) + (1000 * 1000 * (milliseconds % 1000));
        timeToWait_.tv_sec += timeToWait_.tv_nsec / (1000 * 1000 * 1000);
        timeToWait_.tv_nsec %= (1000 * 1000 * 1000);

        active_ = true;
        condition_.signal();
    }

    void Timer::stop()
    {
        Lock lock(conditionMutex_);
        ts_ = 0;
        active_ = false;
        condition_.signal();
    }

    void Timer::run()
    {
        Lock lock(conditionMutex_);

        unsigned int ts;

        while(running_) {

            ts = ts_;

            if(ts > 0) {

                int n = condition_.timedWait(conditionMutex_, timeToWait_);

                if (n == 0) {
                    active_ = false;
                    LOGD("Timer::run(), cancelled");
                } else if (n == ETIMEDOUT) {
                    if(ts_ == ts) {
                        ts_ = 0;
                        Unlock unlock(conditionMutex_);
                        active_ = false;
                        timeout.emit(this);
                    } else {
                        LOGW("Timer::run(), timestamp invalid");
                    }
                } else {
                    LOGE("Timer::run(), unknown result");
                }
            } else {
                condition_.wait(conditionMutex_);
            }
        }
    }

    bool Timer::isRunning()
    {
        Lock lock(conditionMutex_);
        return active_;
    }

    unsigned int Timer::milliseconds(clockType typeOfClock)
    {
    #ifdef __APPLE__
        return nanoseconds() / 1000000LL;
    #else // Android and Linux
        struct timespec now;
        if (clockType::realtime == typeOfClock) {
            clock_gettime(CLOCK_REALTIME, &now);
        } else {
            clock_gettime(CLOCK_MONOTONIC, &now);
        }
        return now.tv_sec * 1000LL + now.tv_nsec / 1000000LL;
    #endif
    }

    float Timer::millisecondsFloat(clockType typeOfClock)
    {
    #ifdef __APPLE__
        return nanoseconds() / 1000000LL;
    #else // Android and Linux
        struct timespec now;
        if (clockType::realtime == typeOfClock) {
            clock_gettime(CLOCK_REALTIME, &now);
        } else {
            clock_gettime(CLOCK_MONOTONIC, &now);
        }
        return (float)(now.tv_sec * 1000LL + now.tv_nsec / 1000000LL);
    #endif
    }

    uint64_t Timer::nanoseconds(clockType typeOfClock)
    {
    #ifdef __APPLE__
        mach_timebase_info_data_t info;
        mach_timebase_info(&info);

        uint64_t tmp = mach_absolute_time();
        tmp *= info.numer;
        tmp /= info.denom;

        return tmp;
    #else // Android and Linux
        struct timespec now;
        if (clockType::realtime == typeOfClock) {
            clock_gettime(CLOCK_REALTIME, &now);
        } else {
            clock_gettime(CLOCK_MONOTONIC, &now);
        }
        return now.tv_sec * 1000000000LL + now.tv_nsec;
    #endif
    }

    uint64_t Timer::autoTimerNanosecs()
    {
#ifdef __BLUEZONE__
        return nanoseconds(); // dft = clockType::monotonic
#else
        return nanoseconds(clockType::realtime);
#endif
    }

    void Timer::sleepMilliseconds(unsigned int ms)
    {
        nanoSleepFromMilliseconds(ms);
    }

    void Timer::nanoSleepFromMilliseconds(unsigned int milliseconds)
    {
        struct timespec tim, tim2;
        tim.tv_sec = milliseconds / 1000;
        tim.tv_nsec = (milliseconds % 1000) * 1000000LL;

        if (nanosleep(&tim , &tim2) < 0 )
        {
            throw Exception("Timer::nanoSleep call to nanosleep() failed.");
        }
    }
}

