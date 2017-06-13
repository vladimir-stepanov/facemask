/**
 * Copyright (C) 2013-2015 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_DUALSCROLLTYPE_HWC_SWAPS_FPS
#define GRAPEVINE_DUALSCROLLTYPE_HWC_SWAPS_FPS

#include <string>

namespace Grapevine
{
    /**
     * @class DualScrollTypeHwcSwapsFpsLogger
     *
     * Performance DB helper class for collecting and logging FPS statistics separately for 2 scroll types: 
     *  1) forceful touch_move(drag-flick), 
     *  2) followup auto-scroll (when finger is released)
     * The statisctics aggregation is done in render() calls that are supposed to be tied to HWC out buffer swaps.
     * So, normally when finger is released from the screen the 'TOUCH' FPS statistics summary is puhsed into logs,
     * Then, when auto-scroll gets complete, the corresponding auto-scroll FPS statistics summary comes.
     * The frame delays (drops) are indicated immediaely (as well as stressed out in the summaries).
     * Usage examples can be found for Launcher/Locker.
     */
    class DualScrollTypeHwcSwapsFpsLogger
    {
    public:
        //CLASS_UNCOPYABLE(DualScrollTypeHwcSwapsFpsLogger);
        /**
         * Constructor that creates a DualScrollTypeHwcSwapsFpsLogger FPS counter.
         *
         * @param [in]  stat_place          Name of the place to count FPS NOTE! 8-char strict (to keep nice log/printing frame!)
         *                                  (Default is: "<|Home|>")
         */
        DualScrollTypeHwcSwapsFpsLogger(std::string statPlace = "<|Home|>");

        /**
         *    Flushes out statisctics (+ does required final summary calc-s) into the Logs summary.
         */
        void flushStatisticsIntoLogs();

        /**
         * Updates FPS statisctics for the mode specified.
         *
         * @param [in]  touch                   'true' if statistics is obtained in touch-scroll mode
         * @param [in]  useExternalTimestamp    'true' if render() timestamp is going to be acompanied with the external timestamp
         *                                      (instead of internal). [Default is: 'false']
         * @param [in]  extTimeStamp            external timestamp.
         * @param [in]  warnSlightlySlowFrame   'true' (default), if warning about a bit slow coming frame has to be logged
         * @param [in]  warnFrameLossDelays     'true' (default), if warning about very slow coming frame has to be logged
         */
        void updatedRenderStatistics(bool touch, bool useExternalTimestamp = false, uint64_t extTimeStamp = 0,
                                    bool warnSlightlySlowFrame = true, bool warnFrameLossDelays = true);

    private:
        bool        timerFlawless, isTouchMove;
        std::string eightCharsWorkplaceName;
        bool        onPause;
        unsigned    frameCnt, totalFrameCnt, slowRenderFrameCnt, frameLossCnt;
        unsigned    totalSlowRenderFrameCntInAutoScroll, totalSlowRenderFrameCntInTouchScroll;
        unsigned    totalFrameLossCntInTouchScroll, totalFrameLossCntInAutoScroll;
        uint64_t    startOfRenderedFrameSetTmStamp, lastRenderedFrameTmStamp;
        uint64_t    totalRenderedFramesTime;
        uint64_t    timeFromPreviousFrameRendered;
    };

} // namespace Grpvn

#endif //GRAPEVINE_DUALSCROLLTYPE_HWC_SWAPS_FPS