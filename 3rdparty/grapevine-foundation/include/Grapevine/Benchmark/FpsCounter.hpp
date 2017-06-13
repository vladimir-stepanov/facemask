/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_BENCHMARK_FPS
#define GRAPEVINE_BENCHMARK_FPS

#include <Grapevine/Core/Class.hpp>
#include <list>
#include <string>

namespace Grapevine
{
    /**
     * @class FpsCounter
     *
     * FpsCounter is a class that can be used to calculate framerates.
     */
    class FpsCounter
    {
    public:
        CLASS_UNCOPYABLE(FpsCounter);

        /**
         * Constructor that creates a FPS counter.
         *
         * @param [in]  name                Name of the counter. Printed to messages.
         * @param [in]  autoPrint           Do we automatically print the FPS.
         * @param [in]  millisecondsT       T between prints.
         * @param [in]  millisecondsTLimit  T limit for step. Amount of
         *                                  steps over the limit and max
         *                                  5 worst times listed in prints.
         */
        FpsCounter(std::string const& name = "FPSCounter",
            bool autoPrint = true,
            unsigned int millisecondsT = 5000,
            unsigned int millisecondsTLimit = 16);

        /**
         * Trigger counter from draw start.
         */
         void startStep();

        /**
         * Trigger counter from draw end.
         */
        void endStep();

    private:
        std::string name_;
        bool autoPrint_;
        unsigned int millisecondsT_;
        unsigned int milliSecondsTStart_;
        unsigned int framesCounted_;
        unsigned int stepStartMilliSeconds_;
        unsigned int milliSecondsUsedForSteps_;
        unsigned int millisecondsTLimit_;
        unsigned int fiveWorstTimes_[5];
        unsigned int stepsOverLimitCount_;
    };
}

#endif
