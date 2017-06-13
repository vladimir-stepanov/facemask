/**
 * Copyright (C) 2013-2015 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

//#include "../../../include/Grapevine/Benchmark/DualScrollTypeHwcSwapsFpsLogger.h"
#include "Grapevine/Benchmark/DualScrollTypeHwcSwapsFpsLogger.hpp"
//#include <string>
#include <assert.h>
#include <Grapevine/Core/Log.hpp>

#include <time.h>

#ifdef __APPLE__
#include <mach/clock.h>
#include <mach/mach.h>
#include <cassert>
#endif

namespace Grapevine
{

#define MAX_CLK_REQ_TRIES   5
#ifndef __APPLE__
#define CLOCK_ACC_TYPE      CLOCK_MONOTONIC  //CLOCK_REALTIME  (no real dif.)
#endif

    // Temp-ly global func-n. TODO: Move for use in specific class only.
    // There are sometimes some timer access issues noticed. Providing a safe func to re-read it in several tries, if needed.
    uint64_t getClkNSecsReliably(bool& ready, bool dropSecs)
    {
        ready = false;
        timespec ts;
        static uint64_t timerTrkSec = 0; // u_int64_t can be either UL or ULL (uint64_t - ULL)

        for(int i=MAX_CLK_REQ_TRIES; !ready && i>0; i--) {
    #ifdef __APPLE__
            clock_serv_t cclock;
            mach_timespec_t mts;
            host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
            ready = (clock_get_time(cclock, &mts) >= 0);
            mach_port_deallocate(mach_task_self(), cclock);
            ts.tv_sec = mts.tv_sec;
            ts.tv_nsec = mts.tv_nsec;
    #else
            ready = (clock_gettime(CLOCK_ACC_TYPE, &ts) >= 0);
    #endif
            if (i<MAX_CLK_REQ_TRIES) { // DB
                LOGW("<--TIMER-->: getClkNSecsReliably(): Several attempts were needed. Read No="
                    " %d (now: sec= %llu, nsec= %lu).", i,
                    static_cast<unsigned long long>(ts.tv_sec), static_cast<unsigned long>(ts.tv_nsec));
            }
        }
        if (dropSecs) {
            timerTrkSec = static_cast<uint64_t>(ts.tv_sec);
            //LOGD("<--TIMER-->: getClkNSecsReliably(): Saved current SECONDS value now: %llu  (nsec= %lu)",
            //        static_cast<unsigned long long>(ts.tv_sec), static_cast<unsigned long>(ts.tv_nsec));
            return static_cast<uint64_t>(ts.tv_nsec);
        } else { // keep track of sec-s to keep linearity of accumulated nsec-s span
            return static_cast<uint64_t>(ts.tv_nsec) + (static_cast<uint64_t>(ts.tv_sec) - timerTrkSec)*1000000000ULL;
        }
    }

#define MAX_WANTEDFPS_FRAME_TIME_NS         16666667ULL     // for 60FPS
#define ALLOWED_DB_DELAY_ADD_NS             600000ULL       // TODO: To be nulled for 'release' profiling (branch defines, e.g. on _DEBUG_, etc.?)
#define FRAME_LOSS_COMPARABLE_DELAY_NS      19000000ULL
#define MAX_GOOD_INTERFRAME_TIME_NS         MAX_WANTEDFPS_FRAME_TIME_NS
#define MIN_TOUCHMOVE_FRAMES_FOR_FPS_STAT   10

    // Performance DB helper class for collecting and logging FPS statistics separately for 2 scroll types (touch_ & auto_)
    DualScrollTypeHwcSwapsFpsLogger::DualScrollTypeHwcSwapsFpsLogger(std::string statPlace) :
        timerFlawless(true),
        isTouchMove(false),
        eightCharsWorkplaceName(statPlace), //inLauncher(profilingLauncher),
        onPause(true),
        frameCnt(0),
        totalFrameCnt(0),
        slowRenderFrameCnt(0),
        frameLossCnt(0),
        totalSlowRenderFrameCntInAutoScroll(0),
        totalSlowRenderFrameCntInTouchScroll(0),
        totalFrameLossCntInTouchScroll(0),
        totalFrameLossCntInAutoScroll(0),
        startOfRenderedFrameSetTmStamp(0),
        totalRenderedFramesTime(0),
        timeFromPreviousFrameRendered(0)
    {
    }

    void DualScrollTypeHwcSwapsFpsLogger::flushStatisticsIntoLogs()
    {
        uint64_t fullRenderedFrameSetTime = 0;
        if (frameCnt > 0) {
            // Calculate global statistics for printing (when enough of local ones, below)
            totalFrameCnt += frameCnt;
            fullRenderedFrameSetTime = (lastRenderedFrameTmStamp - startOfRenderedFrameSetTmStamp);
            assert(startOfRenderedFrameSetTmStamp > lastRenderedFrameTmStamp); // TODO: Change into safer (non-aborting) check, if happens still
            totalRenderedFramesTime += fullRenderedFrameSetTime;
        }

        if (frameCnt > MIN_TOUCHMOVE_FRAMES_FOR_FPS_STAT) {
            // Pre-calculate set statistics for printing
            unsigned avrgStatSetFrameTimeNs = static_cast<unsigned>(fullRenderedFrameSetTime / frameCnt);
            unsigned totalAvrgStatFrameTimeNs = static_cast<unsigned>(totalRenderedFramesTime / totalFrameCnt);
            // LOG-PRINT ALL
            LOGI(" ");
            LOGI(" /||||||||||||||[[[[-[=[ DETAILED %s-FPS-COUNTER ]=]-]]]]||||||||||||||\\",
                    eightCharsWorkplaceName.c_str()); //(inLauncher ? "<|Home|>" : "<Locker>" ));
            LOGI("|| - SET=>  Scroll type: '%s',  Frame count:%5u,  [Slow:%5u],  %s%3u%s",
                (isTouchMove ? "TOUCH" : "Auto-"), frameCnt, slowRenderFrameCnt,
                (frameLossCnt > 0 ? "!!-{FRAME_LOSSES:" : ""), frameLossCnt,
                (frameLossCnt > 0 ? "}-!!" : "    ||"));
            LOGI("||      --==<   Last_SET AVRG: %3.2f ms/frame,    (FPS= %3.3f)   >==--      ||",
                static_cast<float>(avrgStatSetFrameTimeNs)/1.e6f, 1.e9f/static_cast<float>(avrgStatSetFrameTimeNs));
            LOGI("|| --=============================< TOTALS: >=============================-- ||");
            LOGI("|| Total frames:%8u, [Slow:%8u -->  Touch:%7u,  Auto:%7u]  ||",
                totalFrameCnt, totalSlowRenderFrameCntInAutoScroll + totalSlowRenderFrameCntInTouchScroll,
                totalSlowRenderFrameCntInTouchScroll, totalSlowRenderFrameCntInAutoScroll);
            if (totalFrameLossCntInTouchScroll > 0 || totalFrameLossCntInAutoScroll > 0) {
                LOGI("|| ^=-=-> from them !!Frame_LOSSES! : !{Touch:%8u,  Auto:%8u}!!    ||",
                    totalFrameLossCntInTouchScroll, totalFrameLossCntInAutoScroll);
            }
            LOGI("||      --==<   TOTAL:   AVRG: %3.2f ms/frame,    (FPS= %3.3f)   >==--      ||",
                static_cast<float>(totalAvrgStatFrameTimeNs)/1.e6f, 1.e9f/static_cast<float>(totalAvrgStatFrameTimeNs));
            LOGI(" \\|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||/");
            LOGI(" ");
        } else {
            //LOGD("render_PRFL: %s: Too few %s-scroll frames render stat-s collected (%u) to give average FPS "
            //    "statistics for the set.", (inLauncher ? "<|Home|>" : "<Locker>" ), (isTouchMove ? "TOUCH" : "Auto"), frameCnt);
        }
        // Clean all set related stat-s anyway: frame count, ..
        frameCnt = 0;
        slowRenderFrameCnt = 0;
        frameLossCnt = 0;
//             lastRenderedFrameTmStamp = getClkNSecsReliably(timerFlawless, true); // JIC: always updated is needed anyway
//             assert(timerFlawless); // TODO: Change into safer (non-aborting) check, if happens still
//
//             // If ordered to start a new set (e.g. switching touch-auto_scroll), get current time stamp
//             if (startNewSameModeStatisticsSet) {
//                 lastRenderedFrameTmStamp = getClkNSecsReliably(timerFlawless, true);
//                 startOfRenderedFrameSetTmStamp = lastRenderedFrameTmStamp;
//                 //LOGD("render_PRFL: %s: Got 1ST rendered frame time stamp in a NEW SET after flushing out: (%llu)",
//                 //     (inLauncher ? "<|Home|>" : "<Locker>" ), startOfRenderedFrameSetTmStamp);
//                 onPause = false;
//             } else // Set on pause (will have to be cleaned if will be unpaused)
            onPause = true;
    }

    void DualScrollTypeHwcSwapsFpsLogger::updatedRenderStatistics(bool touch, bool useExternalTimestamp, uint64_t extTimeStamp,
                                    bool warnSlightlySlowFrame, bool warnFrameLossDelays)
    {
        // Check don't we have the change of the scroll type.
        // E.g., usual case when it happens normally is when changing from touch to auto-scroll or vise-versa (isn't checked in render())
        if (isTouchMove != touch) {
            // Type of scroll has changed (touch-/auto-) -> flush set stat-s collection and start a new set
            flushStatisticsIntoLogs(); //true);
            isTouchMove = touch; // Just change the current scroll type before leaving
            return;
        }
        // New statistic set has to be started if no on-pause
        if (onPause) {
            // Usual case for sudden auto-scrolls is page changes when dragging an icon
            // Reset all the stuff for the start
            frameCnt = 0;
            slowRenderFrameCnt = 0;
            frameLossCnt = 0;
            lastRenderedFrameTmStamp = (useExternalTimestamp ? extTimeStamp : getClkNSecsReliably(timerFlawless, true));
            assert(timerFlawless); // TODO: Change into safer (non-aborting) check, if happens still
            startOfRenderedFrameSetTmStamp = lastRenderedFrameTmStamp;
            onPause = false;
            return;
        }
        // New statistic accepted and to be added to current set collection if no on-pause
        uint64_t timeStampNow = (useExternalTimestamp ? extTimeStamp : getClkNSecsReliably(timerFlawless, true));
        assert(timerFlawless); // TODO: Change into safer (non-aborting) check, if happens still
        timeFromPreviousFrameRendered = timeStampNow - lastRenderedFrameTmStamp;
        assert(lastRenderedFrameTmStamp > timeStampNow); // TODO: Change into safer (non-aborting) check, if happens still
        lastRenderedFrameTmStamp = timeStampNow;
        frameCnt++;

        if (timeFromPreviousFrameRendered > MAX_WANTEDFPS_FRAME_TIME_NS + ALLOWED_DB_DELAY_ADD_NS) {
            // "Slow frame" case
            slowRenderFrameCnt++;
            if (isTouchMove) {
                totalSlowRenderFrameCntInTouchScroll++;
            } else {
                totalSlowRenderFrameCntInAutoScroll++;
            }
            // VERY NOTICEABLE unacceptable time/frame loss
            if (timeFromPreviousFrameRendered > FRAME_LOSS_COMPARABLE_DELAY_NS) {
                frameLossCnt++;
                if (isTouchMove) {
                    totalFrameLossCntInTouchScroll++;
                } else {
                    totalFrameLossCntInAutoScroll++;
                }
                // TODO: We can count also percentage of lost time compared to properly rendered
                //       (or number of potentially lost frames for wanted 60 fps)
                if (warnSlightlySlowFrame) {
                    // Report the "UNACCEPTABLY Slow frame" FRAME LOSS
                    LOGW("render_PRFL: %s: !!! BADLY SLOW FRAME (FRAME LOSS) in %s-scroll [FrameN_inSet= %d, InterFrameTime= %3.2f ms] !!!",
                        eightCharsWorkplaceName.c_str()/*(inLauncher ? "<|Home|>" : "<Locker>" )*/, (isTouchMove ? "TOUCH" : "Auto"), frameCnt,
                        static_cast<float>(timeFromPreviousFrameRendered)/1.e6f);
                }
            } else if (warnSlightlySlowFrame) {
                // Report the "Slightly slow frame"
                //LOGW("render(): Home: !! SLOW FRAME in %s-scroll [FrameNumberInASet= %d, ActualFrameTime= %llu, timeStamp= %llu] !!",
                //    (isTouchMove ? "TOUCH" : "Auto"), frameCnt, timeFromPreviousFrameRendered, lastRenderedFrameTmStamp);
                LOGW("render_PRFL: %s A bit SLOW FRAME in %s-scroll [FrameN_inSet= %d, InterFrameTime= %3.2f ms]!",
                    eightCharsWorkplaceName.c_str()/*(inLauncher ? "<|Home|>" : "<Locker>" )*/, (isTouchMove ? "TOUCH" : "Auto"), frameCnt,
                    static_cast<float>(timeFromPreviousFrameRendered)/1.e6f);
            }
        }
    }

} // namespace Grpvn
