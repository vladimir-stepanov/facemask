/**
 * Copyright (C) 2013-2015 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CORE_TIMER_HPP
#define GRAPEVINE_CORE_TIMER_HPP

#include "Grapevine/Core/Class.hpp"
#include "Grapevine/Core/Thread.hpp"
#include "Grapevine/Core/Mutex.hpp"
#include "Grapevine/Core/Condition.hpp"
#include <time.h>
#include <stdint.h> // For uint64_t
#include <cstddef>
using std::ptrdiff_t; // NOTE: sigc++ has problem without this line!
#include <sigc++/signal.h>
#include <pthread.h>

namespace Grapevine
{
    /**
     * @class Timer
     *
     * Timer is a helper class which can provide timestamps and sleep functionality.
     */
    class Timer: public Thread
    {
    public:
        CLASS_UNCOPYABLE(Timer)

        sigc::signal<void, Timer*> timeout;

        /**
         * @enum clockType  Defines the type of clock used in clock_gettime().
         *                  Currently, the CLOCK_REALTIME and CLOCK_MONOTONIC are supported.
         *                  Depending on the system, peripherial device events timestamping done by the corresponding kernel
         *                  driver appears to fall into CLOCK_REALTIME (P8) or CLOCK_MONOTONIC (N5) family. Hence, the corresponding
         *                  clock type slection is the precondiotin for the best effor of measuring time differences to such the events.
         */
        enum clockType {
            monotonic   = 0,    /**< refers to CLOCK_MONOTONIC (used in N5),     is coded as uint8_t of value 0 */
            realtime    = 1,    /**< refers to CLOCK_REALTIME  (used in P8),     is coded as uint8_t of value 1 */
        };

        Timer();
        ~Timer();

        /**
         * Starts timer. Timer's timeout signal will be emitted after the
         * given time. If timer is called more than once, the old timeouts
         * still on-going will be removed.
         */
        void start(unsigned int milliseconds);

        /**
         * Stops timer, removes all on-going timeouts.
         */
        void stop();

        /**
         * @return boolean which reflects the timer running state.
         */
        bool isRunning();

        bool operator==(Timer const& r);

        /**
         * @param [in]  typeOfClock type of clock to be used to extract the value
         *                          (default = clockType::monotonic).
         * @return                  milliseconds. Returned value is not promised to be related
         *                          to anything like process or system start time etc. So values
         *                          are useful only for example when looking time differences.
         */
        static unsigned int milliseconds(clockType typeOfClock = clockType::monotonic);

        /**
         * @param [in]  typeOfClock type of clock to be used to extract the value
         *                          (default = clockType::monotonic).
         * @return                  milliseconds. Same as above but using float for more
         *                          precise purposes.
         *
         @note result is based on nanosecond resolution which is converted to milliseconds.
         */
        static float millisecondsFloat(clockType typeOfClock = clockType::monotonic);

        /**
         * @param [in]  typeOfClock type of clock to be used to extract the value
         *                          (default = clockType::monotonic).
         * @return                  nanoseconds. Returned value is not promised to be related
         *                          to anything like process or system start time etc. So values
         *                          are useful only for example when looking time differences.
         */
        static uint64_t nanoseconds(clockType typeOfClock = clockType::monotonic);

        /**
         * @return nanoseconds. Returned value is automatically selected (build time) to
         *     be based on the CLOCK_REALTIME (P8) or CLOCK_MONOTONIC (N5) so, depending on
         *     the corresponding peripherial device events timestamping by kernel driver,
         *     hence representing the best choice for measuring time differences to such the
         *     system events.
         */
        static uint64_t autoTimerNanosecs();

        static void sleepMilliseconds(unsigned int milliseconds);

    protected:
        void run();

    private:
        static void nanoSleepFromMilliseconds(unsigned int milliseconds);

        // For timeouts:
        bool running_;
        bool active_;
        static unsigned int lastId_;
        unsigned int id_;
        struct timespec timeToWait_;
        unsigned int ts_;
        Mutex conditionMutex_;
        Condition condition_;
    };
}

#endif
