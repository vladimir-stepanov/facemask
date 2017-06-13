/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Benchmark/FpsCounter.hpp"
#include <Grapevine/Core/Timer.hpp>
#include <Grapevine/Core/Log.hpp>

namespace Grapevine
{
    FpsCounter::FpsCounter(std::string const& name,
        bool autoPrint,
        unsigned int millisecondsT,
        unsigned int millisecondsTLimit):

        name_(name),
        autoPrint_(autoPrint),
        millisecondsT_(millisecondsT),
        milliSecondsTStart_(0),
        framesCounted_(0),
        stepStartMilliSeconds_(0),
        milliSecondsUsedForSteps_(0),
        millisecondsTLimit_(millisecondsTLimit),
        stepsOverLimitCount_(0)
    {
        for (unsigned int i = 0; i < 5; i++)
        {
            fiveWorstTimes_[i] = 0;
        }
    }

    void FpsCounter::startStep()
    {
        stepStartMilliSeconds_ = Timer::milliseconds();
    }

    void FpsCounter::endStep()
    {
        unsigned int currentTime = Timer::milliseconds();
        unsigned int stepTimeDiff = currentTime - stepStartMilliSeconds_;
        milliSecondsUsedForSteps_ += stepTimeDiff;
        framesCounted_++;

        // Check if ended step is in five worst ones
        for(unsigned int i = 0; i < 5; i++)
        {
            if(fiveWorstTimes_[i] < stepTimeDiff)
            {
                for(unsigned int ii = 4; ii > i && ii >= 1; ii--)
                {
                    fiveWorstTimes_[ii] = fiveWorstTimes_[ii - 1];
                }
                fiveWorstTimes_[i] = stepTimeDiff;
                break;
            }
        }

        if (stepTimeDiff > millisecondsTLimit_)
        {
            stepsOverLimitCount_++;
        }

        if ((milliSecondsTStart_ + millisecondsT_) < currentTime)
        {
            if (framesCounted_ == 0)
            {
                LOGI("%s: av.frame(ms):?, FPS(max):?, No frames counted!");
            }
            else
            {
                float averageFrameDrawInMilliSeconds = (float)milliSecondsUsedForSteps_ / (float)framesCounted_;
                float averageTimeBetweenFrames = (float)(currentTime - milliSecondsTStart_) / (float)framesCounted_;
                if (autoPrint_ == true)
                {
                    LOGI("%s: av.T/s[T/s]:%5.1f, av.T[ms]:%5.1f, av.start-end/s[T/s]:%5.1f, limit[ms]:%d:, overLimit/steps:%d/%d, TOP5[ms]:(%d %d %d %d %d)",
                        name_.c_str(),
                        1000.0f / averageTimeBetweenFrames,
                        averageFrameDrawInMilliSeconds,
                        1000.0f / averageFrameDrawInMilliSeconds,
                        millisecondsTLimit_,
                        stepsOverLimitCount_,
                        framesCounted_,
                        fiveWorstTimes_[0],
                        fiveWorstTimes_[1],
                        fiveWorstTimes_[2],
                        fiveWorstTimes_[3],
                        fiveWorstTimes_[4]);
                }
            }

            framesCounted_ = 0;
            milliSecondsUsedForSteps_ = 0;
            milliSecondsTStart_ = currentTime;

            // Worst times
            for (unsigned int i = 0; i < 5; i++)
            {
                fiveWorstTimes_[i] = 0;
            }
            stepsOverLimitCount_ = 0;
        }
    }
}
