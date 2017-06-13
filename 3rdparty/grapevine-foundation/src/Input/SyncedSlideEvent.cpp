/**
 * Copyright (C) 2013-2015 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

// Since recent that's needed for LOGD to function
// (BZ: ngos-meta/api/include/Grapevine/Core/Log.hpp | YZ: frameworks/api/include/Grapevine/Core/Log.hpp)
// FIXME: Adjust when some general config place is defined
#ifndef DEBUG
 //#define DEBUG
#endif

//#define ALLOW_POLY_PREDICTIONS
#ifdef ALLOW_POLY_PREDICTIONS
// Uncomment the define below for the case of huge 1.5frame+ input event pipeline jams on P8 (tmp solution until delays fixed).
// NOTE: In normal case it's not recommended to unlock (unless additional logic added for downgrading the selection of approx
//       polynom order in use) since high order polynoms show bad prediction capabilities (lower should be better, but still unperfect)
//#define ALLOW_1FRAME_ULTD_POLYPREDICTION
#endif

#ifdef DEBUG
 // It is expected that due to prediction math (geom/2 shift auto-slowdown regression), the rendering will stop before reaching this number.
 #define MAX_PREDICTION_TIME_NS 9 * 16667000 //(last is FRAME_TIME_NS)
#endif

#include <math.h>
#include <Grapevine/Input/SyncedSlideEvent.hpp>
#include <Grapevine/Core/Log.hpp>

namespace Grapevine
{

    SyncedSlideEvent::SyncedSlideEvent() :
        arrStPtr_(0),
        arrEnPtr_(0),
        lastCIdx_(0),
        usePiecewiseLinearApprx_(true),
        renderSequenceWasPaused_(false),
        regressionCurveGrows_(true),
        inPredictionState_(false),
        prevPosReported_(0.1e10f),          // DFT to provide big diff for sure
        renderSequencePausedPos_(0.1e10f),  // DFT to provide big diff for sure
        prevIsNotUltdPolyPredicted_(true),
        maxAprxPolynPwr(3)                  // JIC: both error protection and indication
    {
        aprxTchPosArr_[0].timeStamp_tms = 0;
    }


    void SyncedSlideEvent::InitTouchTrackDataUs(float touchPos, uint64_t touchTimeEventUs, uint64_t touchEventRegTimeNs,
                                                float delayMsForSlidePosApprxCalc, apprxMode typeOfApprx)
    {
        switch (typeOfApprx){
            case apprxMode::piecewsLinear :
                usePiecewiseLinearApprx_ = true;
                break;

            case apprxMode::polynomial :
                usePiecewiseLinearApprx_ = false;
                maxAprxPolynPwr = MAX_POLYN_ORDER;
                break;

            case apprxMode::polynomial_sq :
                usePiecewiseLinearApprx_ = false;
                maxAprxPolynPwr = MAX_SQUARE_POLYN_ORDER;
                break;

            default :
                usePiecewiseLinearApprx_ = true;
        }

        // Register delay for slide pos approximation
        delayNsForSlidePosApprxCalc_ = static_cast<uint64_t>(delayMsForSlidePosApprxCalc * 1000000.0f); // ms -> ns

        if (delayMsForSlidePosApprxCalc < 0) { // TODO: define clearly what limits are in force
            delayNsForSlidePosApprxCalc_ = 1000; //1 us
            LOGE("(||SYNC-TCH||) SySld: InitTouchTrackDataUs: ERROR: !! delayMsForSlidePosApprxCalc provided is negative (%f ms). "
                 "Minimal allowed delay is used instead: %llu ns !!", delayMsForSlidePosApprxCalc, delayNsForSlidePosApprxCalc_);
        } else if (delayMsForSlidePosApprxCalc > 2.f * FRAME_TIME_MS) {
            delayNsForSlidePosApprxCalc_ = static_cast<uint64_t>(1.7f * 1000000.0f * FRAME_TIME_MS); //ns
            LOGE("(||SYNC-TCH||) SySld: InitTouchTrackDataUs: ERROR: !! delayMsForSlidePosApprxCalc provided is unacceptably too high (%f ms). "
                 "Maximal allowed delay is used instead: %llu(us) !!", delayMsForSlidePosApprxCalc, delayNsForSlidePosApprxCalc_);
        }

        hugeBackTimeShiftNs_ = 0;
        arrStPtr_ = arrEnPtr_ = 0;
        // Drop all the time from the timestamps before the touch-slide start in question (to keep preciseness of calc-s in float range)
        //  (Observed that the delay of getting signal about an event to get through reaches inter-frame time!)
        touchDownEventUs_ = touchTimeEventUs;
        uint64_t touchDownEventNs = touchDownEventUs_ * 1000U;
        //touchDownRegTimeStampNs_ = touchEventRegTimeNs; // touch down registration time (bk for possible time shift fixing)

        // INIT Sanity check: Correspondence of the 1st touch down event internal timestamp and its registration time (the latter
        //                    shall be [just a bit] later!!). Unfortunately, we cannot be sure about fixing too late reg time case.
        // Reason: Different kernel drivers provide event timestamping using principally different clocks (system dependent), so the
        //         timestamps might appear to be totally inconsistent with time snapshots taken for producing time comparison logic
        //         based requests. So, we have to be ready to cope with this as well..
        if (touchDownEventNs > touchEventRegTimeNs) {
            hugeBackTimeShiftNs_ = touchDownEventNs - touchEventRegTimeNs + delayNsForSlidePosApprxCalc_;
            LOGE("(||SYNC-TCH||) SySld: InitTouchTrackDataUs() ERROR: the touchEventRegTimeNs=%llu (ns) specified in the init is%s before "
                 "the actual touchDownEventUs_= %llu. BackTimeShiftNs: %llu ns (will be saved and used later in order to sustain the logic).",
                 touchEventRegTimeNs, (hugeBackTimeShiftNs_ > 1000000U ? " far" : ""), touchDownEventUs_, hugeBackTimeShiftNs_);
        }
        // Replicate into the fake 'previous' touch point
        inTchPosArr_[0].timeStamp_us = 0;
        inTchPosArr_[0].pos = static_cast<fpformat>(touchPos);
        inTchPosArr_[MAX_FIFO_SIZE - 1].timeStamp_us = 0;
        inTchPosArr_[MAX_FIFO_SIZE - 1].pos = inTchPosArr_[0].pos;
        // ChePolyno exrtas mostly
        accumBufSize_ = 1;
        inPredictionState_ = ltdPolyPrediction_ = false;
        LOGD("(||SYNC-TCH-I||) SySld: __Done init touch-mv apprx buf:__");
        LOGD("(||SYNC-TCH-I||) SySld:                     (TouchDwnPos =%f)  <[| touchDownUs =%llu |]>", touchPos, touchDownEventUs_);
        LOGD("(||SYNC-TCH-I||) SySld: ^^Done init touch-mv apprx buf:^^");
    }


    bool SyncedSlideEvent::updateTouchMoveEventsUsFifo(float inPos, uint64_t eventTimeStampUs)
    {
        // Keeping internal time stamps in ms (FP!) is intended against prcision lost in FP computations with short mantissa
        //  byte width (e.g. float). Note: The real event time stamp resolution is microsec-s (us).
        uint64_t timeStampRel_us = eventTimeStampUs - touchDownEventUs_;
        fpformat pos =  static_cast<fpformat>(inPos);

        // Drop the auto-regression prediction mode
        if (ltdPolyPrediction_) {
            ltdPolyPrediction_ = false;
            lastPolySlwDwnPos_ = pos; // JIC use last real input for new regression boundary cal-n input
        }
#ifdef DEBUG
        // DB only check (not really needed for any logic now?)
        if (!(!arrStPtr_ && 0 == inTchPosArr_[0].timeStamp_us)) {
            // If not in the initial start point, check the tendency kept
            if (regressionCurveGrows_) {
                if (pos < inTchPosArr_[arrStPtr_].pos) {
                    regressionCurveGrows_ = false;
                    LOGD("(||SYNC-TCH||) SySld: Changed monotonicity into Decreaing. Stopping..?");
                }
            } else {
                if (pos > inTchPosArr_[arrStPtr_].pos) {
                    regressionCurveGrows_ = true;
                    LOGD("(||SYNC-TCH||) SySld: Changed monotonicity into Increasing. Stopping..?");
                }
            }
        }
        // DB only check (not noticed anymore in real-life testing)
        if (eventTimeStampUs < touchDownEventUs_) {
            LOGE("(||SYNC-TCH||) SySld: ERR: !! TimeStamp of event attempted to be registered (%llu us) is before the intitial touch down event: %llu us !!",
                eventTimeStampUs, touchDownEventUs_);
        } else if (timeStampRel_us < inTchPosArr_[arrStPtr_].timeStamp_us) {
            LOGE("(||SYNC-TCH||) SySld: ERR: ! Relative timestamp (us) of 'previous' event (Prev=%f) is after current event (Curr =%llu). Overwriting previous !",
                inTchPosArr_[arrStPtr_].timeStamp_us, timeStampRel_us);
        } else
#endif
        // This is really valid (truly noticed in Locker in real-life testing), and hence, very important guarding check
        if (timeStampRel_us - inTchPosArr_[arrStPtr_].timeStamp_us < REAL_TIMESTAMP_DIFF_US) {
            LOGE("(||SYNC-TCH||) SySld: ERR-!: Difference in relative timestamps of latest reg-d and coming event is negligible "
                "(Prev[I=%u]<Tm=%llu(us), Pos=%f>, Coming=<Tm=%llu(us), Pos=%f>). We have to overwrite the previous (BAD ISSUES WITH MATH!). "
                "If not done, NaNs'n'Excp-s will show up!!",
                arrStPtr_, inTchPosArr_[arrStPtr_].timeStamp_us, inTchPosArr_[arrStPtr_].pos, timeStampRel_us, pos);
            // No pre-increment - means writing over old one
        }
        else
        {
            // Step array buffers' ptr-s
            ++arrStPtr_&= MAX_FIFO_SIZE_MASK; //optimized with masking (faster than DIV): & 0xF; //<- for 16 only
            // ChePolyno extras
            if (accumBufSize_ < MAX_APX_DATA_CNT_IN_USE) {
                accumBufSize_++;
            } else {
                ++arrEnPtr_ &= MAX_FIFO_SIZE_MASK;
            }
        }
        // Register new pos value
        inTchPosArr_[arrStPtr_].timeStamp_us    = timeStampRel_us;
        inTchPosArr_[arrStPtr_].pos             = pos;
#ifdef DEBUG
        if (!usePiecewiseLinearApprx_) {
            LOGD("(||SYNC-TCH||) SySld: ChePolynoAprx-mode Full DB printout of collected touch-move events: (| arrEnPtr_ =%u, arrStPtr_ =%u |) "
                "after adding new: Pos=%6.1f Time=%9llu(us). accumBufSize_=%u", arrEnPtr_, arrStPtr_, pos, timeStampRel_us, accumBufSize_);
            for (uint8_t i = 0; i < MAX_FIFO_SIZE; i++) {
                LOGD("(||SYNC-T-in||)  I=%02u %s TIME =%9llu : POS =%10.5f", i,
                    (i == arrEnPtr_ ? "__>__" : (i == arrStPtr_ ? "^^>^^" : " --> ")),
                    inTchPosArr_[i].timeStamp_us, inTchPosArr_[i].pos);
            }
        }
#endif
        // Out -token to re-initiate render sequence, if it was paused before
        if (renderSequenceWasPaused_ && FP_ABS(renderSequencePausedPos_ - pos) > MIN_SCENE_SHIFT_TO_REDRAW) {
            renderSequenceWasPaused_ = false;
            return true;
        } else {
            return false;
        }
    }


    float SyncedSlideEvent::getSlidePosNowNs(uint64_t timeRequestedNs, bool& needContinueRendering)
    {
        // Here's where the bit width of timeStamp value squeezes down
        uint64_t timeRequestedAdjusted_ns = timeRequestedNs + hugeBackTimeShiftNs_ - touchDownEventUs_ * 1000U - delayNsForSlidePosApprxCalc_;
        fpformat ret;
        bool     requestedTimeIsOutOfBufTimeRange = false;
        bool     negativeReqTime = (timeRequestedNs + hugeBackTimeShiftNs_ < touchDownEventUs_ * 1000U + delayNsForSlidePosApprxCalc_);

        LOGD("(||SYNC-TCH||) SySld: getSlidePosNowNs entry -->  timeRequestedNs= %llu(ns), touchDownEventUs_= %llu(us), timeRequestedAdjusted_ns: %llu(ns) %s.",
             timeRequestedNs, touchDownEventUs_, timeRequestedAdjusted_ns, (negativeReqTime ? "[last is NEGATIVE and needs fixing!]" : ""));
#ifdef DEBUG
        if (hugeBackTimeShiftNs_) {
            LOGE("(||SYNC-TCH||) SySld: getSlidePosNowNs() ERROR fix calc-n mode: by the original touchEventRegTimeNs specified in the init "
                 "being%s before the actual touchDownEventUs_ the BackTimeShiftNs was estimated as: %f (so used now in order to sustain the logic).",
                 (hugeBackTimeShiftNs_ > 1000000U ? " far" : ""), hugeBackTimeShiftNs_);
        }
        // Request-time sanity check
        // Forward prediction. With gradual time distance increase (while still not crossing next touch node / neighbor pixel),
        //  supposed to be handled by regression (keep it for later?)
        if (!negativeReqTime && timeRequestedAdjusted_ns > inTchPosArr_[arrStPtr_].timeStamp_us * 1000U + MAX_PREDICTION_TIME_NS) {
            LOGW("(||SYNC-TCH||) SySld: getSlidePosNowNs() ERROR in input cannot be fixed: position is requested for the time which "
                 "is FAR BEHIND THE LAST value registered in our accumulated approximation buffer: [relative adjusted: reqTime= "
                 "%llu(ns). lastReg-ed= %llu(us)]. Pushing out the last reg-ed in our buf: (Pos= %f).",
                 timeRequestedAdjusted_ns, inTchPosArr_[arrStPtr_].timeStamp_us, inTchPosArr_[arrStPtr_].pos);
            requestedTimeIsOutOfBufTimeRange = true;
            ret = inTchPosArr_[arrStPtr_].pos; // TODO: Assign the last reg-d value instead of trying to predict too far ahead. OK?
        } else
#endif
        if (1 == accumBufSize_) {
            // Real-life bug case in Locker where it gets 'down'-events in the middle of swipe
            LOGE("(||SYNC-TCH||) SySld: getSlidePosNowNs(): ERROR in the input cannot be really fixed: position is requested when there "
                "is just the initial touch-down event is the only registered in the buffer so far. !!INVALID case!! CHECK THE EVENT LOGIC! "
                "Approximation output cannot be requested before 1st move position is registered (after initial touch_down)!!! "
                "[relative adjusted: reqTime= %llu(ns). TchDownReg-ed= %llu(us)]. Pushing out the touch-down position from our buf: (Pos= %f).",
                timeRequestedAdjusted_ns, inTchPosArr_[arrEnPtr_].timeStamp_us, inTchPosArr_[arrEnPtr_].pos);
            ret = inTchPosArr_[arrEnPtr_].pos;
        } else
        // Unexpected (even taking into account hugeBackTimeShiftNs_) back shift - request for over-outdated past
        if (negativeReqTime || timeRequestedAdjusted_ns < inTchPosArr_[arrEnPtr_].timeStamp_us * 1000U) {
            // Auto-shrinking logic for delayTMsForSlidePosApprxCalc_ for (hopefully, initial, so still short) buffer even start point miss.
            // TODO: Complete the auto-shrinking logic for delayTMsForSlidePosApprxCalc_!
            LOGW("(||SYNC-TCH||) SySld: getSlidePosNowNs(): MESS in the input cannot be reliably fixed: position is requested "
                "for the time which is BEFORE THE 1ST value registered in our accumulated move positions buffer (which can be "
                "initial touch-down event even). (May be it has sense to DECREASE THE APPRX.POINT DELAY?) [relative adjusted (us): "
                "reqTime= %llu%s lastReg-ed= %llu, TchDownReg-ed= %llu]. Collected moves buf size = %u.", timeRequestedAdjusted_ns,
                (negativeReqTime ? " is NEGATIVE." : "."), inTchPosArr_[arrEnPtr_].timeStamp_us, touchDownEventUs_, accumBufSize_);
            if (accumBufSize_ < 3) {
                uint64_t newTmpDelayedTsNs = timeRequestedNs + hugeBackTimeShiftNs_ - touchDownEventUs_ * 1000U
                                            - (delayNsForSlidePosApprxCalc_ >> 1); // Try half-way smaller
                bool     negAutoShrnkdReqTime = (timeRequestedNs + hugeBackTimeShiftNs_ < touchDownEventUs_ * 1000U
                                              + (delayNsForSlidePosApprxCalc_ >> 1));
                if (!negAutoShrnkdReqTime && newTmpDelayedTsNs >= inTchPosArr_[arrEnPtr_].timeStamp_us * 1000U) { // Lucky
                    LOGW("(||SYNC-TCH||) SySld: getSlidePosNowNs(): Attempt (allowed for newborn buffers like this: of size=%u) to fix "
                        "the MESS in the input^: THE GIVEN (in Init) APPRX.POINT DELAY = %llu(ns) is auto-DECREASED twice (to %llu ns). "
                        "So, now request time gets AFTER THE the initial touch-down event: %llu (ns). Requesting the approximated "
                        "value for the timepoint with auto-shrunk delay then.",
                        accumBufSize_, delayNsForSlidePosApprxCalc_, (delayNsForSlidePosApprxCalc_ >> 1), newTmpDelayedTsNs);
                    // Request approximation for the timepoint with the half-shrunk delay. So, we'll show at least some reaction to touch-input
                    if (usePiecewiseLinearApprx_) {
                        // Piecewise 1st order linear interpolation
                        ret = getPiecewiseLinearInterpltnOut(newTmpDelayedTsNs);
                    } else {
                        // Chebyshev Orthogonal Polynomial ("Parabolic") Approximation curve building (updating) and
                        //  extraction/calculation of pos for the corresponding timestamp from that
                        ret = getChePolynoOut(accumBufSize_, newTmpDelayedTsNs);
                    }
                } else { // Unlucky
                    LOGE("(||SYNC-TCH||) SySld: getSlidePosNowNs(): The MESS in the input cannot be fixed: position is requested "
                        "for the time which is BEFORE THE 1ST value registered in our accumulated move positions buffer (or the "
                        "initial touch-down event) even after DECREASING THE APPRX.POINT DELAY TWICE! "
                        "(%llu -> %llu). Returning the touch-down position from our buf: (Pos= %f).",
                        delayNsForSlidePosApprxCalc_, (delayNsForSlidePosApprxCalc_ >> 1), inTchPosArr_[arrEnPtr_].pos);
                    ret = inTchPosArr_[arrEnPtr_].pos;
                }
            } else {
                LOGE("(||SYNC-TCH||) SySld: getSlidePosNowNs(): The MESS in the input cannot be fixed: position is requested "
                    "for the time which is BEFORE THE 1ST value registered in our accumulated move positions buffer (or initial "
                    "touch-down event even). And the collected moves buf size is too big already for approx.delay auto-shrinking= "
                    "%u. Pushing out the 1st kept in our buf: (Pos= %f).", accumBufSize_, inTchPosArr_[arrEnPtr_].pos);
                ret = inTchPosArr_[arrEnPtr_].pos;
            }
        } else {
            // Normal timestamp request: in range of collected points
            // (Although, perfect case for math would be having it in the middle of the buf. However, as we perfer having minimal
            // delays, behind last element is generally OK.)
            if (usePiecewiseLinearApprx_) {
                // Piecewise 1st order linear interpolation
                ret = getPiecewiseLinearInterpltnOut(timeRequestedAdjusted_ns);
            } else {
                // Chebyshev Orthogonal Polynomial ("Parabolic") Approximation curve building (updating) and
                //  extraction/calculation of pos for the corresponding timestamp from that
                ret = getChePolynoOut(accumBufSize_, timeRequestedAdjusted_ns);
            }
        }

        // Out
        // TODO: Update if will be changed. On practice (N5/P8), we cannot see more than 1 pixel difference in touch inputs currently
        if (requestedTimeIsOutOfBufTimeRange) {
            // TODO: Check and decide about the best behavior in case of requestIsOutOfBufTimeRange.
            //       Currently below: attempt to increase the probability of pausing the redraw in such the cases.
            renderSequenceWasPaused_ = FP_ABS(prevPosReported_ - ret) < MIN_SCENE_SHIFT_TO_REDRAW * 2;
        } else {
            renderSequenceWasPaused_ = FP_ABS(prevPosReported_ - ret) < MIN_SCENE_SHIFT_TO_REDRAW; // as in auto-scroll
        }
        if (renderSequenceWasPaused_) {
            needContinueRendering = false;
            renderSequencePausedPos_ = ret;
        } //else needContinueRendering = true;  // NOTE: uncomment, if need forced overwriting of the token (to 'true') at every call

        LOGD("(||SYNC-TCH||) SySld: getSlidePosNowNs <-- out for timeRequestedNs= %llu(ns) {Delayed: %llu(ns)} [actual internal adj-d "
            "(-TchDwn -Delay)ReqTS= %llu(ns) <hugeBackTimeShiftNs_= %llu>%s | PrevReqTS= %llu(ns)], {[aprxOutPos =%f]} (PrevAprxOutPos= %f)%s.",
            timeRequestedNs, timeRequestedNs - delayNsForSlidePosApprxCalc_, timeRequestedAdjusted_ns,
            hugeBackTimeShiftNs_, (negativeReqTime ? " (! ISSUE: negativeReqTime !)" : ""), prevPosRepRelTmStmpNs_, ret,
            prevPosReported_, (renderSequenceWasPaused_ ? " ['render_pause' is signaled, as too small shift]" : ""));
        prevPosReported_ = ret;
        // TODO: If in use by PWA, branch to and newTmpDelayedTsNs as well as provide overflow check (with negativeReqTime/negAutoShrnkdReqTime)
        prevPosRepRelTmStmpNs_ = timeRequestedAdjusted_ns;

        return static_cast<float>(ret);
    }


    fpformat SyncedSlideEvent::getPiecewiseLinearInterpltnOut(uint64_t adjustedTimeOfReqNs)
    {
        // Piecewise 1st order linear interpolation
        uint8_t closestPrevTmStmArrIdx = (arrStPtr_ ? arrStPtr_ - 1 : MAX_FIFO_SIZE - 1);
        uint8_t closestNextTmStmArrIdx = arrStPtr_;
        uint8_t chkdClsPrevTmStmArrIdx = closestPrevTmStmArrIdx;
        uint8_t chkdClsNextTmStmArrIdx = closestNextTmStmArrIdx;
        fpformat ret = 0;
        fpformat timeOfReqUs = adjustedTimeOfReqNs * US_TO_MS_MULT; // same 1e-3 const

        // If no dif in input pos
        if (inTchPosArr_[closestNextTmStmArrIdx].timeStamp_us * 1000U < adjustedTimeOfReqNs) {
            LOGD("(||SYNC-TCH||) SySld: NO INPUT TOUCH-POINT move event came for duration of input event delay time. "
                "Trying to make simple fast prediction.");
            // Cannot use piece-wise interpolation - no measurement ahead (we're beyond the last piece).
            // PREDICT then.. -=-=-=->
            if (FP_ABS(inTchPosArr_[closestNextTmStmArrIdx].pos - inTchPosArr_[closestPrevTmStmArrIdx].pos) <= .5f) { // min diff is 1.0
                // No dif in Pos between neighbor touchpoints (let's say PrevPos == NextPos).
                LOGD("(||SYNC-TCH||) SySld: No dif in Pos between neighbor touchpoints (last and previous): %f. Predictive "
                    "calc-ns (based on earlier regressiion) can be last hope to get a return value different from previous: %f.",
                        inTchPosArr_[closestNextTmStmArrIdx].pos, prevPosReported_);
            }
            // NOTE: Can we dynamically relax acceptable delay in this case ?  -->  goes into more fuzzy logic field.. (just use ChePolyno below then..)
            if (renderSequenceWasPaused_) {
                // Just returned from pause and the event that revoked us is behind
                ret = inTchPosArr_[closestNextTmStmArrIdx].pos;
                if (FP_ABS(ret - prevPosReported_) <= 1.0f) {
                    ret = prevPosReported_;
                    LOGD("(||SYNC-TCH||) SySld: Prediction is impossible - we've got break before "
                        "and were just revoked. However, pos distance of last return and last received is < 1, "
                        "so just repeating last return (to prevent ringing): %f", ret);
                } else {
                    LOGD("(||SYNC-TCH||) SySld: Prediction is impossible - we've got break before "
                        "and were just revoked. (Pos distance of last return and last received is > 1). "
                        "So just directly returning last received pos: %f", ret);
                }
            } else { // No return from break/pause on the step before
                // Check, didn't we skip a level in coef-s pair, so we have to shift
                if (closestNextTmStmArrIdx != lastCIdx_) { // if Coef-s are not in-line
                    fpformat uK = (inTchPosArr_[closestNextTmStmArrIdx].pos - inTchPosArr_[closestPrevTmStmArrIdx].pos) /
                            static_cast<fpformat>(inTchPosArr_[closestNextTmStmArrIdx].timeStamp_us
                                                - inTchPosArr_[closestPrevTmStmArrIdx].timeStamp_us);
                    fpformat uC = inTchPosArr_[closestNextTmStmArrIdx].pos
                                - uK * static_cast<fpformat>(inTchPosArr_[closestNextTmStmArrIdx].timeStamp_us);
                    if (closestPrevTmStmArrIdx == lastCIdx_) { // if just one level behind
                        prevK_ = K_;
                        prevC_ = C_;
                        K_ = uK;
                        C_ = uC;
                        lastCIdx_ = closestNextTmStmArrIdx;
                        LOGD("(||SYNC-TCH||) SySld: Synced Coef-s set pair (one step up) to (prevK_=%f, prevC_=%f | K_=%f, C_=%f)",
                            prevK_, prevC_, K_, C_);
                    } else {  // all coefs need to be calc-ed from scratch
                        K_ = uK;
                        C_ = uC;
                        uint8_t closestPrePrevTmStmArrIdx =
                                    (closestPrevTmStmArrIdx ? closestPrevTmStmArrIdx - 1 : closestPrevTmStmArrIdx - 1);
                        prevK_ = (inTchPosArr_[closestPrevTmStmArrIdx].pos - inTchPosArr_[closestPrePrevTmStmArrIdx].pos) /
                                static_cast<fpformat>(inTchPosArr_[closestPrevTmStmArrIdx].timeStamp_us
                                                    - inTchPosArr_[closestPrePrevTmStmArrIdx].timeStamp_us);
                        prevC_ = inTchPosArr_[closestPrevTmStmArrIdx].pos
                                - prevK_ * static_cast<fpformat>(inTchPosArr_[closestPrevTmStmArrIdx].timeStamp_us);
                        LOGD("(||SYNC-TCH||) SySld: Recreated Coef-s set pair to (prevK_=%f, prevC_=%f | K_=%f, C_=%f)",
                            prevK_, prevC_, K_, C_);
                    }
                }
                // Check possible stall case
                if (FP_ABS(K_) < .1e-4f) { // TODO: Recheck the order after time order change again
                    // Stall state. There no sense to calc anything - just return the last manifest coef and wait for being paused
                    ret = C_;
                    LOGD("(||SYNC-TCH||) SySld: Prediction is static as we had no change (Stall state: K=~0). Returning last "
                        "clac-ed manifest as pos=C_: %f", ret);
                } else  // Not in this kind of stall state now
                if (0 != prevK_ && prevK_ * K_ < 0) { // Check, can the coef-s be used for slowdown safely
                    // Dif signs in coefs - no normal case can be expected
                    // TODO: or PREVIOUS to be returned?
                    ret = inTchPosArr_[closestNextTmStmArrIdx].pos;
                    LOGD("(||SYNC-TCH||) SySld: Coef signs (so, regr-n direction) changed [prevK_=%f, K_=%f], cannot predict. "
                        "Returning last collected pos: %f. [PrevRet=%f]", prevK_, K_, ret, prevPosReported_);
                } else {
                    // Predictive calculations are required still, if got here
                    // Get the direction of movement
                    fpformat drctMult = 0;
                    fpformat dlt = 0;
                    if (inTchPosArr_[closestNextTmStmArrIdx].pos > inTchPosArr_[closestPrevTmStmArrIdx].pos) {
                        drctMult = 1.0f;
                        if (K_ > 0) {
                            dlt = prevK_ - K_;
                        }
                        else LOGW("(||SYNC-TCH||) SySld: Calc-d data mismatch: reg-ed pos was growing while last calc-ed K decreasing!");
                    } else if (inTchPosArr_[closestNextTmStmArrIdx].pos < inTchPosArr_[closestPrevTmStmArrIdx].pos) {
                        drctMult = -1.0f;
                        if (K_ < 0) {
                            dlt = K_ - prevK_;
                        }
                        else LOGW("(||SYNC-TCH||) SySld: Calc-d data mismatch: reg-ed pos was decreasing while last calc-ed K growing!");
                    }
                    fpformat newK = K_ - dlt;
                    fpformat newC;
                    if (inPredictionState_ || FP_ABS(prevK_) < FP_ABS(K_)) { //fabsf(K_) < fabsf(newK)  is checked later
                        // If made at least one prediction before  or  If coef-s were not flattering
                        ret = K_ * timeOfReqUs + C_;
                        LOGD("(||SYNC-TCH||) SySld: 2 Coef sets collected are not really reliable (either have already been used for prediction "
                                "at least once before   or  coef-s are not flattering). Trying to use the last coef-s , though..");
                        newK = K_;
                        newC = C_;
                    } else {
                        // Expected normal prediction case
                        if (K_ * newK < 0) {
                            // Coeff-s changed direction - nulling main coeff then
                            newK = 0;
                            LOGD("(||SYNC-TCH||) SySld: After applying Dlt, we got direction change, so nulling the K now (get stalled).");
                            ret = newC = inTchPosArr_[closestNextTmStmArrIdx].pos;
                        } else {
                            // Most expected normal case
                            newC = inTchPosArr_[closestNextTmStmArrIdx].pos
                                    - newK * static_cast<fpformat>(inTchPosArr_[closestNextTmStmArrIdx].timeStamp_us);
                            ret = newK * timeOfReqUs + newC;
                        }
                        LOGD("(||SYNC-TCH||) SySld: Simple fast NORM.PREDICTION: line regression (prevK_=%f, prevC_=%f "
                            " | K_=%f, C_=%f), dlt=%f: [Pos(%f) = K(%f) * TIME(%f) + C(%f)]  <PrevRet=%f>",
                            prevK_, prevC_, K_, C_, dlt, ret, newK, timeOfReqUs, newC, prevPosReported_);
                    }
                    // FINAL: Check the case of possible crossing of the limiting border
                    if (FP_ABS(ret - inTchPosArr_[closestNextTmStmArrIdx].pos) > 1.0f) {  // min diff is 1.0
                        // Checked that last regression should have crossed next touch position before, if kept unchanged
                        LOGD("(||SYNC-TCH||) SySld:  If the coef-s kept, this leads to CROSSING the next touch pos "
                            "level: %f. So, re-interpolating..  (Last received pos: %f) [PrevRet=%f]",
                            ret, inTchPosArr_[closestNextTmStmArrIdx].pos, prevPosReported_);
                        // Just take the midpoint
                        if (inPredictionState_) { // if were already in half-step approach
                            ret = prevPosReported_ + 0.5f * (inTchPosArr_[closestNextTmStmArrIdx].pos + drctMult - prevPosReported_);
                            // Adjust coeff-s - make impression we've derived those with the same normal calc-s
                            newK = (ret - prevPosReported_) /
                                    (static_cast<fpformat>(adjustedTimeOfReqNs - prevPosRepRelTmStmpNs_) * US_TO_MS_MULT);
                            newC = ret - newK * timeOfReqUs;
                        } else { // just + 1/2 over last reg-ed
                            ret = inTchPosArr_[closestNextTmStmArrIdx].pos + 0.5f * drctMult;
                            // Adjust coeff-s - make impression we've derived those with the same normal calc-s
                            newK = (ret - inTchPosArr_[closestNextTmStmArrIdx].pos) /
                                (timeOfReqUs - static_cast<fpformat>(inTchPosArr_[closestNextTmStmArrIdx].timeStamp_us));
                            newC = ret - newK * timeOfReqUs;
                        }
                        // Adjust coeff-s - make impression we've derived those with the same normal calc-s
                        newK = (ret - prevPosReported_) / (static_cast<fpformat>(adjustedTimeOfReqNs - prevPosRepRelTmStmpNs_) * US_TO_MS_MULT);
                        newC = ret - newK * timeOfReqUs;
                        LOGD("(||SYNC-TCH||) SySld: Simple fast PREDICTION was LIMITED: line: (prevK_=%f, prevC_=%f | K_=%f, C_=%f), dlt=%f: "
                            " [Pos(%f) = K(%f) * TIME(%f) + C(%f)]  <PrevRet=%f, PrevTmsmp=%llu(ns)>",
                            prevK_, prevC_, K_, C_, dlt, ret, newK, timeOfReqUs, newC, prevPosReported_, prevPosRepRelTmStmpNs_);
                    } else {
                        LOGD("(||SYNC-TCH||) SySld:  If the coef-s kept, this should still ensure non-crossing "
                            "the next touch pos level: %f.", ret);
                    }
                    // Shift the coef-s
                    prevK_ = K_;
                    K_ = newK;
                    prevC_ = C_;
                    C_ = newC;
                    lastCIdx_ = closestNextTmStmArrIdx;
                }
            }
            inPredictionState_ = true;
            // Updated indexes (predicted): +1
            chkdClsPrevTmStmArrIdx = closestNextTmStmArrIdx;
        } else {
            // We have at least 1 event reg-ed ahead the checkpoint ----->
            // No PREDICTION is needed (just linear interpolation on correct segment)
            // Thus, search for FIFO indexes with closest timestamps wrapping previous frame
            // Reason: on P8/N5 (currently) during vivid touch-moves, input events come
            //  ~2-4 times during interframe delay (with the target frame rate currently = 60 FPS)
            // However, with checkpoint delay currently used (~8ms) we've never seen more
            //  than 1 input event ahead the checkpoint in the FIFO so far, but JIC..:
            for(; inTchPosArr_[chkdClsPrevTmStmArrIdx].timeStamp_us * 1000U > adjustedTimeOfReqNs; ) {
                chkdClsPrevTmStmArrIdx = (chkdClsPrevTmStmArrIdx ? chkdClsPrevTmStmArrIdx - 1 : MAX_FIFO_SIZE - 1);
                LOGW("(||SYNC-TCH||) SySld: With the interpolation point delay we use (-%f) we have to skip 1 input event back (to %u-th)",
                        timeOfReqUs, chkdClsPrevTmStmArrIdx);
            }
            chkdClsNextTmStmArrIdx = (chkdClsPrevTmStmArrIdx + 1) & MAX_FIFO_SIZE_MASK;
            prevK_ = K_; // Shift 2 set FIFO of saved coef-s
            prevC_ = C_;
            inPredictionState_ = false;
            // Solve linear equation system of 1st order  (0 dev protection is the 1st branching condition).
            K_ = (inTchPosArr_[chkdClsNextTmStmArrIdx].pos - inTchPosArr_[chkdClsPrevTmStmArrIdx].pos) /
                    static_cast<fpformat>(inTchPosArr_[chkdClsNextTmStmArrIdx].timeStamp_us - inTchPosArr_[chkdClsPrevTmStmArrIdx].timeStamp_us);
            C_ = inTchPosArr_[chkdClsNextTmStmArrIdx].pos - K_ * static_cast<fpformat>(inTchPosArr_[chkdClsNextTmStmArrIdx].timeStamp_us);
            ret = K_ * timeOfReqUs + C_;
            lastCIdx_ = closestNextTmStmArrIdx;
            LOGD("(||SYNC-TCH||) SySld: Getting prev.frame pos from obtained line regression: Pos(%f) = K(%f) * Time(%f) + C(%f)",
                    ret, K_, timeOfReqUs, C_);
        }
#ifdef DEBUG
        bool haveShift = (chkdClsPrevTmStmArrIdx != closestPrevTmStmArrIdx);
        outDbBiFifo[chkdClsPrevTmStmArrIdx].pos = ret;
        outDbBiFifo[chkdClsPrevTmStmArrIdx].timeStamp_us = adjustedTimeOfReqNs; //!! ns is instead of us here !!
        // Full DB print out
        LOGD("(||SYNC-TCH||) SySld: Full DB printout of collected touch-move events: <<| PrevIdx =%u, LastElmIdx =%u |>>",
                closestPrevTmStmArrIdx, closestNextTmStmArrIdx);
        for(uint8_t i = 0;  i < MAX_FIFO_SIZE;  i++) {
            LOGD("(||SYNC-T||)  I=%02u    %s TIME =%9llu    POS =%10.5f", i,
                (i == closestPrevTmStmArrIdx ? "__-->__" : (i == closestNextTmStmArrIdx ? "^^-->^^" : "  -->  ")),
                inTchPosArr_[i].timeStamp_us, inTchPosArr_[i].pos);
            // Calc-s/predictions now
            // LOG only if fresh (timestamp suites the wrapping boundaries' ones)
            if ((i < MAX_FIFO_SIZE - 1 && inTchPosArr_[i].timeStamp_us * 1000U <= outDbBiFifo[i].timeStamp_us
                && outDbBiFifo[i].timeStamp_us <= inTchPosArr_[i + 1].timeStamp_us * 1000U) ||
                (MAX_FIFO_SIZE -1 == i && inTchPosArr_[i].timeStamp_us * 1000U <= outDbBiFifo[i].timeStamp_us
                && outDbBiFifo[i].timeStamp_us <= inTchPosArr_[0].timeStamp_us * 1000U) ||
                (haveShift && (chkdClsPrevTmStmArrIdx == i || closestPrevTmStmArrIdx == i))) {
                if (inPredictionState_) {
                    LOGD("(||SYNC-T||)          %s   timeReq =%12llu Pos =%10.5f %s", (chkdClsPrevTmStmArrIdx == i ? ">>" : "  "),
                        outDbBiFifo[i].timeStamp_us, outDbBiFifo[i].pos, ((haveShift && chkdClsPrevTmStmArrIdx == i) ? " [! PREDICTED !]" : ""));
                } else {
                    LOGD("(||SYNC-T||)          %s   timeReq =%12llu Pos =%10.5f %s", (chkdClsPrevTmStmArrIdx == i ? ">>" : "  "),
                        outDbBiFifo[i].timeStamp_us, outDbBiFifo[i].pos, ((haveShift && chkdClsPrevTmStmArrIdx == i) ? " [<--shifted to]" :
                        ((haveShift && closestPrevTmStmArrIdx == i) ? " ( shifted from )" : "")));
                }
            }
        }
#endif
        return ret;
    }


    fpformat SyncedSlideEvent::getChePolynoOut(uint8_t curBufSize, uint64_t adjustedTimeOfReqNs)
    {
        fpformat    rtrn                    = 0;
        uint8_t     checkedMaxPolynomOrder  = 0;
        uint64_t    adjRelativeTimeOfReqUs = static_cast<fpformat>(adjustedTimeOfReqNs) * US_TO_MS_MULT; // same 1e-3

        adjRelativeTimeOfReqUs += (adjustedTimeOfReqNs - adjRelativeTimeOfReqUs * 1000U > 0 ? 1 : 0);  // To be sure in comparisons to us

        // Limit the max apprx polynom
        //  From the new touch we buf growing 2->MAX_APX_DATA_CNT_IN_USE(/MAX_FIFO_SIZE)
        if (curBufSize > maxAprxPolynPwr) {  //or instead of curBufSize can use directly: accumBufSize_;
            checkedMaxPolynomOrder = maxAprxPolynPwr;
        } else if (curBufSize < 2) { // TODO: Comm-out this check as extra. Shall be checked before!!!
            LOGE("(||SYNCT||) SySld: curBufSize < 2. No apprx can be done for this amount of data collected. "
                "The valid minimal actual buf size mub >=2. CHECK THE CAUSE OF THE CASE!!");
            checkedMaxPolynomOrder = 0; // nither approximation, nor prediction can be done in this case, so no any further setup is needed
            // Abnormal case: (we shouldn't get asked for moved pos while not collected at least 2 points)
            rtrn = prevPosReported_ = inTchPosArr_[arrStPtr_].pos;
            LOGW("(||SYNC-T||) SySld: With actualCollctdBufSize == 1, the single input Pos value can be only returned: %f !", rtrn);
            return rtrn;
        } else {
            checkedMaxPolynomOrder = curBufSize - 1;
        }

        // Enough buf size for making approximations/predictions
        LOGD("(||SYNC-TCH||) SySld: Starting ChePolyno calc-s on src end-Ptr [%u] (st-Ptr=%u) of collected buf size: %u. "
            "checkedMaxPolynomOrder = %u", arrEnPtr_, arrStPtr_, accumBufSize_, checkedMaxPolynomOrder);

        bool        needCalculatingApprxOut  = true;
        bool        startOfLtdPolyPrediction = false;

        // Limitations (inevitable for predictions with low order polynomial, the further prediction - the more)
        if (adjRelativeTimeOfReqUs > inTchPosArr_[arrStPtr_].timeStamp_us) {
            // PREDICTION case
#ifdef ALLOW_1FRAME_ULTD_POLYPREDICTION   // (for the case of huge 1.5frame+ input event pipeline jams on P8).
            // Check if the most recent input is for more than one frame time before the requested time-point (or polynomial order is too weak for predictions)
            if (adjRelativeTimeOfReqUs > inTchPosArr_[arrStPtr_].timeStamp_us + FRAME_TIME_US || checkedMaxPolynomOrder < 2) {
                // Employ the liner bisecting limitation logic only for second+ frame prediction
                // (The case of huge 1.5frame+ input event pipeline delays on P8).
                // We expect at the next frame we can still be behind the last event reg-ed, but there should be the new ones.
#endif
                // Logic for limiting predictions similar to simple linear ->
                //  we cannot exceed 1 point shift from last reg-ed event pos, if no new event came during frame time that change the pos by 1 or more.
                if (ltdPolyPrediction_) { // staying in limited prediction
                    rtrn = (lastPolySlwDwnPos_ + activePolyLmt_) * FP_HLF_MLT;
                    lastPolySlwDwnPos_ = rtrn;
                    needCalculatingApprxOut = false;
                    LOGD("(||SYNCT||) SySld: Continue giving LIMITED PREDICTED value (%f) instead of calc-ing ChePolynoApprx.", rtrn);
                } else {
                    // Just entered prediction mode
                    ltdPolyPrediction_ = true;
                    startOfLtdPolyPrediction = true;
                    LOGD("(||SYNCT||) SySld: Entering now LIMITED PREDICTION mode starting from still calc-ing initials by ChePolynoApprx.");
#ifndef ALLOW_POLY_PREDICTIONS
                    needCalculatingApprxOut = false;
                    if (inTchPosArr_[arrStPtr_].pos <= aprxTchPosArr_[accumBufSize_ - 2].pos &&
                        ((accumBufSize_ <= 2) || (inTchPosArr_[arrStPtr_].pos <= aprxTchPosArr_[accumBufSize_ - 3].pos &&
                        aprxTchPosArr_[accumBufSize_ - 2].pos <= aprxTchPosArr_[accumBufSize_ - 3].pos))) {
                        // We're heading towards decreasing pos (or stall already)
                        activePolyLmt_ = inTchPosArr_[arrStPtr_].pos - 1;
                    } else {
                        // We're heading towards increasing pos
                        activePolyLmt_ = inTchPosArr_[arrStPtr_].pos + 1;
                    }
                    rtrn = (lastPolySlwDwnPos_ + activePolyLmt_) * FP_HLF_MLT;
                    LOGD("(||SYNCT||) SySld: ChePolynoApprx has to FAST START giving LIMITED PREDICTED pos SEQUENCE:  Starting (from: %f |> "
                        "towards: %6.1f).  |lastPolySlwDwnPos_ =%f|", rtrn, activePolyLmt_, lastPolySlwDwnPos_);
                    lastPolySlwDwnPos_ = rtrn;
#endif
                }
#ifdef ALLOW_1FRAME_ULTD_POLYPREDICTION
            } else {
                // Allow using unlimited poly- predicted value during delay within one frame only, but assert the token.
                // (E.g., the case of huge 1.5frame+ old input event pipeline jams on P8).
 #ifdef DEBUG
                if (adjRelativeTimeOfReqUs - inTchPosArr_[arrStPtr_].timeStamp_us > FRAME_TIME_US) {
                    LOGW("(||SYNCT||) SySld: ChePolynoApprx is expected to give its UNLIMITED PREDICTED Pos for req-ed time: %llu ns "
                        "(which is behind the last reg-ed Pos %f in time [%llu us], FOR MORE THAN THAN 1 frame time. <--ERROR! CHECKED!).",
                        adjustedTimeOfReqNs, inTchPosArr_[arrStPtr_].pos, inTchPosArr_[arrStPtr_].timeStamp_us);
                } else {
                    LOGI("(||SYNCT||) SySld: ChePolynoApprx is expected to give its UNLIMITED PREDICTED Pos for req-ed time: %llu ms "
                        "(which is behind the last reg-ed Pos %f in time [%llu us], but still less than 1 frame time. CHECKED!).",
                        adjustedTimeOfReqNs, inTchPosArr_[arrStPtr_].pos, inTchPosArr_[arrStPtr_].timeStamp_us);
                }
 #endif
                ltdPolyPrediction_ = false;
                prevIsNotUltdPolyPredicted_ = false;
            }
#endif
        } else {
            // NORMAL case (we have at least one position registered with timestamp ahead of the requested time)
            ltdPolyPrediction_ = false;
            prevIsNotUltdPolyPredicted_ = true;
        }

        if (needCalculatingApprxOut) {
            LOGD("(||SYNC-TCH||) SySld: Starting ChePolyno approx-n calc-ns on src end-Ptr [%u] (st-Ptr=%u) of collected buf size: %u. "
                "checkedMaxPolynomOrder = %u (as in normal approx mode - not a simple geom.regression).",
                    arrEnPtr_, arrStPtr_, accumBufSize_, checkedMaxPolynomOrder);

            // Init all arrays needed and get + eliminate the newly added const time value from the final time request
            uint64_t newAddedConstTimeToTchDwnUs = InitPolyBaseAndGetCurTimePedestal(curBufSize, checkedMaxPolynomOrder);
            uint64_t normalizedTimeOfReqNs = adjustedTimeOfReqNs - newAddedConstTimeToTchDwnUs * 1000U;

            // Calculate the approximated polynom coef-s values and select pos based on the polynom providing best results for known data
            rtrn = UpdateAprxPolynCoefMtxAndGetPos(checkedMaxPolynomOrder, normalizedTimeOfReqNs);

            if (startOfLtdPolyPrediction) {
                // Check the trend(values general mutation direction) we're proceeding according to..
                // NOTE: From real-life stat-s: In reality, when we get the break/pause/delay in getting coming inputs, most probably
                //       it refers to reverting of the movement direction (not slowing down the current as assumed below). So,..
                //       TODO: M/b provide the logic accordingly (insead of 'slow-down')?
                //if (inTchPosArr_[arrStPtr_].pos > lastPolySlwDwnPos_) { // were heading towards decreasing pos
                // Check the purely input trend (TODO: Consider m.b. more robust A[1]+A[2]~A[2]+A[3] appended with accumBufSize_ safety logic?)
                if (inTchPosArr_[arrStPtr_].pos <= aprxTchPosArr_[accumBufSize_ - 2].pos &&
                    ((accumBufSize_ <= 2) || (inTchPosArr_[arrStPtr_].pos <= aprxTchPosArr_[accumBufSize_ - 3].pos &&
                    aprxTchPosArr_[accumBufSize_ - 2].pos <= aprxTchPosArr_[accumBufSize_ - 3].pos))) {
                    // We're heading towards decreasing pos (or stall already)
                    activePolyLmt_ = inTchPosArr_[arrStPtr_].pos - 1;
                    if ((rtrn > lastPolySlwDwnPos_ && rtrn > aprxTchPosArr_[accumBufSize_ - 2].pos) ||
                        // ^Predicted value is going to cross the upper limit (== previous returned value) (while supposed going down). Swap to the hard limit.
                        rtrn < activePolyLmt_) { // Predicted value is already going to cross the bottom limit (direction down). Swap to the hard limit.
                        LOGD("(||SYNCT||) SySld: ChePolynoApprx is preparing to START giving LIMITED PREDICTED pos SEQUENCE in "
                            "decreasing trend where poly-predicted value (%f) cannot be used as falls beyond the limits.", rtrn);
#ifdef ALLOW_1FRAME_ULTD_POLYPREDICTION
                        if (!prevIsNotUltdPolyPredicted_) {  // ALLOW_1FRAME_ULTD_POLYPREDICTION case only
                            rtrn = activePolyLmt_ + FP_HLF_MLT; // or still take some dependent on poly- predicted rtrn?
                        } else
#endif
                            rtrn = (lastPolySlwDwnPos_ + activePolyLmt_) * FP_HLF_MLT;  // FIXME: Check how much the lastPolySlwDwnPos_ can be trusted after InClear logic added?
                    }
                    //else LOGD("(||SYNCT||) SySld: ChePolynoApprx is preparing to START giving LIMITED PREDICTED pos SEQUENCE in decreasing "
                    //        "trend where poly-predicted value (%f) is used as starting point as it's within the allowed +-1+ pixel limits.", rtrn);
                } else {
                    // We're heading towards increasing pos
                    activePolyLmt_ = inTchPosArr_[arrStPtr_].pos + 1;
                    if ((rtrn < lastPolySlwDwnPos_ && rtrn < aprxTchPosArr_[accumBufSize_ - 2].pos) ||
                        // ^Predicted value is going to cross the lower limit (== previous returned value) (while supposed going up). Swap to the hard limit.
                        rtrn > activePolyLmt_) { // Predicted value is already going to cross the ceiling limit (direction up). Swap to the hard limit.
                        LOGD("(||SYNCT||) SySld: ChePolynoApprx is preparing to START giving LIMITED PREDICTED pos SEQUENCE in "
                            "increasing trend where poly-predicted value (%f) cannot be used as falls beyond the limits.", rtrn);
#ifdef ALLOW_1FRAME_ULTD_POLYPREDICTION
                        if (!prevIsNotUltdPolyPredicted_) {  // ALLOW_1FRAME_ULTD_POLYPREDICTION case only
                            rtrn = activePolyLmt_ - FP_HLF_MLT; // or still take some dependent on poly- predicted rtrn?
                        } else
#endif
                            rtrn = (lastPolySlwDwnPos_ + activePolyLmt_) * FP_HLF_MLT;  // FIXME: Check how much the lastPolySlwDwnPos_ can be trusted after InClear logic added?
                    }
                    //else LOGD("(||SYNCT||) SySld: ChePolynoApprx is preparing to START giving LIMITED PREDICTED pos SEQUENCE in increasing "
                    //        "trend where poly-predicted value (%f) is used as starting point as it's within the allowed +-1+ pixel limits.", rtrn);
                }
                LOGD("(||SYNCT||) SySld: ChePolynoApprx has to START giving LIMITED PREDICTED pos SEQUENCE:  Starting (from: %f |> "
                    "towards: %6.1f).  |lastPolySlwDwnPos_ =%f|", rtrn, activePolyLmt_, lastPolySlwDwnPos_);
            } else if (!prevIsNotUltdPolyPredicted_) {
                LOGD("(||SYNCT||) SySld: Calc-ed approx.value is an UNLIMITED PREDICTED value as mentioned earlier ^.");
            }
            lastPolySlwDwnPos_ = rtrn;
        }
        return rtrn;
    }


    uint64_t SyncedSlideEvent::InitPolyBaseAndGetCurTimePedestal(uint8_t actualCollctdBufSize, uint8_t apxPlnMxPwr)
    {
        // Preparation to ChePolyno Arrays (coef-s matrix/ces) creation
        // TODO: Update accordingly for "running"(recurrent) case
        for(uint8_t y = 0;  y <= apxPlnMxPwr;  y++) {
            for(uint8_t i = 0;  i <= apxPlnMxPwr + 1;  POrChePolynoMtx_[y][i] = ApxPlnCoefMtx_[y][i] = 0, i++);
        }
        for(uint8_t d = 0;  d < actualCollctdBufSize;  PhiCntd_[0][d++] = 1); //Fi(0) on "x"-axis
        Denm_[0] = actualCollctdBufSize; //0-th denominator
        uint64_t spareConstTimeVal_us = 0;

        // Clone-convert the required input FIFO buf array part into the approximation dedicated with normalizing the input FIFO
        //  element's timestamps (oldest element should have timestamps==0). If we do not have the full FIFO collected, it's ==0 by default.
        if (actualCollctdBufSize >= MAX_APX_DATA_CNT_IN_USE) {
            spareConstTimeVal_us = inTchPosArr_[arrEnPtr_].timeStamp_us;
            LOGD("(||SYNC-TCH||) SySld: Shifting out extra time from the fully filled-up buf: spareConstTimeVal_us =%llu.", spareConstTimeVal_us);
        } else {
#ifdef DEBUG  // Mark specifically all the values that in theory might take part in approximation
            for(uint8_t n = 1;  n < MAX_APX_DATA_CNT_IN_USE;  n++) {
                aprxTchPosArr_[n].pos = 0;
                aprxTchPosArr_[n].timeStamp_tms = 0;
            }
#endif
        }
        //aprxTchPosArr_[0].timeStamp_tms = 0;  // <-- This (oldest element t-stamp) shall be kept like that by dft (after init in the c-tor, e.g.)
        aprxTchPosArr_[0].pos = inTchPosArr_[arrEnPtr_].pos;  // Oldest element is copied with normalized to timestamp value ==0
        LOGD("(||SYNC-TCH||) SySld: Converting input buf into the approximation one: 0-s element: InBuf[%u] =PosVal =%f, TmStmp =%llu tms (mub ==0!).",
             arrEnPtr_, aprxTchPosArr_[0].pos, aprxTchPosArr_[0].timeStamp_tms);
        for(uint8_t bufPtr = (arrEnPtr_ + 1) & MAX_FIFO_SIZE_MASK, n = 1;  n < actualCollctdBufSize;
            bufPtr++, bufPtr &= MAX_FIFO_SIZE_MASK, n++) {
            aprxTchPosArr_[n].pos = inTchPosArr_[bufPtr].pos;
            aprxTchPosArr_[n].timeStamp_tms = static_cast<fpformat>(inTchPosArr_[bufPtr].timeStamp_us - spareConstTimeVal_us) * US_TO_TMS_MULT;
            LOGD("               [%02u]:{%02u} | <PosVal=%6.1f, TmStmp=%9.5f>", n, bufPtr, aprxTchPosArr_[n].pos, aprxTchPosArr_[n].timeStamp_tms);
        }

        LOGD("(||SYNC-TCH||) SySld: Ready to start filling up new ChePolyno matrices: apxPlnMxPwr=%u, actualCollctdBufSize=%u "
            "(mub == accumBufSize_=%u), arrEnPtr_=%u, arrStPtr_=%u.", apxPlnMxPwr, actualCollctdBufSize, accumBufSize_, arrEnPtr_, arrStPtr_);
        // Creating (or TODO: updating for recurrent case) base (orthogonal "parabollic" Chebyshev) polynomial arrays (polynoms' coef-s matrix)
        UpdateChePolynoMatrix(apxPlnMxPwr);
        return spareConstTimeVal_us;
    }


    void SyncedSlideEvent::UpdateChePolynoMatrix(uint8_t activeApxPlnMaxPwr)
    {
        // TODO: Check does it have sense to provide recurrent("running")-way summ-s culc-s optimization.
        // The reality is such that for normal move we have to deliver apprx-d out after 2-3 inputs. Only slow motion decrease that down to 1(0).
        uint8_t     n, i0, i1, i2;
        fpformat    tSum, NumrSum, PhiSum, K;

        const fpformat divConst = (1.0f / static_cast<fpformat>(accumBufSize_));

        POrChePolynoMtx_[0][0] = 1;
        // Main loop of ChePolynoMtx forming
        for(i0 = 1;  i0 <= activeApxPlnMaxPwr;  i0++) {
            POrChePolynoMtx_[i0][i0] = 1; // Set 1st coef for the top order arg.
            // Coef for 0-th power.
            for(tSum = 0, n = 1;  n < accumBufSize_;  //NOTE: n = 0; <- for normal case (when aprxTchPosArr_[0].timeStamp_tms!=0)
                tSum += FP_POW(aprxTchPosArr_[n].timeStamp_tms, i0), n++);
            // For 1st order polynom, nothing else is counted. -> if(i0>1)
            POrChePolynoMtx_[i0][0] = -tSum * divConst;
            // Rest of (non-bounding) coefficients culc-n loop. (NOTE: For 1st [i0==1] pass, the loop skips. So, don't care about uninit-d Denm_[1])
            for(i1 = 1;  i1 < i0;  i1++) {
                // Calculating coefficient on which *-ed i1-th from the formed polynoms.
                for(NumrSum = 0, n = 1;  n < accumBufSize_;   //NOTE: n = 0; <- for normal case (when aprxTchPosArr_[0].timeStamp_tms!=0)
                    NumrSum += PhiCntd_[i1][n] * FP_POW(aprxTchPosArr_[n].timeStamp_tms, i0), n++);
                K = -NumrSum / Denm_[i1];  // Calculating ratio, => coefficient. (NOTE: ! Division here ! double-check 0/undef-s !)
                // * i1-th polynom with coefficient and + with i0-th polynom under calculation.
                for(i2 = 0;  i2 <= i1;  POrChePolynoMtx_[i0][i2] += POrChePolynoMtx_[i1][i2] * K, i2++);
            }
            // Calc i0-th "denominator" Denm-> [Phi_^2]
            // (NOTE: Below, "PhiCntd_[i0][0]=0, n=1" is for our only special case when we keep aprxTchPosArr_[0].timeStamp_tms==0)
            //for(Denm_[i0] = 0, PhiCntd_[i0][0] = 0, n = 1;  n < accumBufSize_;  Denm_[i0] += FP_POW(PhiSum, 2), n++) {  //FAILURE!  TODO: Check why and try to save some CPU cycles still
            for(Denm_[i0] = 0, n = 0;  n < accumBufSize_;  Denm_[i0] += FP_POW(PhiSum, 2), n++) {
                for(PhiSum = 0, i1 = 0;  i1 <= i0;
                    PhiSum += POrChePolynoMtx_[i0][i1] * FP_POW(aprxTchPosArr_[n].timeStamp_tms, i1), i1++);
                PhiCntd_[i0][n] = PhiSum;
            }
        }
    }


#define WEIGHT_COEF_FOR_PRELAST_ELEM_APXERR  2.1f
#define WEIGHT_COEF_FOR_LAST_ELEM_APXERR     2.3f

    fpformat SyncedSlideEvent::UpdateAprxPolynCoefMtxAndGetPos(uint8_t actAprxPlnMaxOrder, uint64_t normalizedTimeOfReqNs)
    {
        uint8_t     n, i0, i1;
        fpformat    normTimeTMs = normalizedTimeOfReqNs * NS_TO_TMS_MULT;
        fpformat    ySum, NumrSum;
        fpformat    Ckf[MAX_FIFO_SIZE];
        // Approximation error arrays and the coef are calc-ed to be used for output selection currently (+ DB)
        fpformat    ErrPst[MAX_POLYN_ORDER + 1], ErrNgt[MAX_POLYN_ORDER + 1], cErr;
        fpformat    ErrPreLast[MAX_POLYN_ORDER + 1], ErrLast[MAX_POLYN_ORDER + 1];

        // Main loop of polynomial coef-s array forming
        for(i0 = 0;  i0 <= actAprxPlnMaxOrder;  i0++) {
            // Calculate coef for a Chebyshev polynom.
            for(NumrSum = 0, n = 0;  n < accumBufSize_;
                NumrSum += PhiCntd_[i0][n] * aprxTchPosArr_[n].pos, n++);
            Ckf[i0] = NumrSum / Denm_[i0];  // Calculate ratio, => Ckf.+
            // Creating next of final approximating polynoms.
            // i0-th approximating polynom = Chebyshev polynom * Ckf + (i0 - 1)-th approximating.
            ApxPlnCoefMtx_[i0][i0] = Ckf[i0];
            for(i1 = 0;  i1 < i0;  ApxPlnCoefMtx_[i0][i1] = ApxPlnCoefMtx_[i0-1][i1] + POrChePolynoMtx_[i0][i1] * Ckf[i0], i1++);
        }

        fpformat   AprxdPointsMtx[MAX_POLYN_ORDER + 1][MAX_FIFO_SIZE];
        // Par-s to calculate aggregate approximation error.
        fpformat    minErr                  = 1.1e9f;
        fpformat    minErrEndFocused        = 1.1e9f;
        uint8_t     bstPlnOrdr              = actAprxPlnMaxOrder;
        uint8_t     bstPlnOrdrEndFocused    = actAprxPlnMaxOrder;
        fpformat    calcCmpTmp;

        for(uint8_t j = 1;  j <= actAprxPlnMaxOrder;  j++) {
            LOGD(" Rslt.aprx.polynom of order #%u:\n [%10.5f] + [%10.5f]*x + [%10.5f]*x^2 + [%e]*x^3 + [%e]*x^4;", j,
                ApxPlnCoefMtx_[j][0], ApxPlnCoefMtx_[j][1], ApxPlnCoefMtx_[j][2], ApxPlnCoefMtx_[j][3],
                ApxPlnCoefMtx_[j][4], ApxPlnCoefMtx_[j][5], ApxPlnCoefMtx_[j][6]);
            for(ErrPst[j] = ErrNgt[j] = 0, n = 0;  n < accumBufSize_;  n++) {
                for(ySum = ApxPlnCoefMtx_[j][0], i1 = 1;  i1 <= j;
                    ySum += ApxPlnCoefMtx_[j][i1] * FP_POW(aprxTchPosArr_[n].timeStamp_tms, i1), i1++); // Phi_(x)
                AprxdPointsMtx[j][n] = ySum; // Save the approximated positions -> TODO: Draw DB graph-s? ;)
                if (ySum > aprxTchPosArr_[n].pos) {
                    cErr = ySum - aprxTchPosArr_[n].pos;
                    ErrPst[j] += cErr;
                    LOGD("  [%02u] | <RlVal=%6.1f, TmStmp=%7.3f>  (apxVal=%10.5f, apxErr=%10.6f)",
                        n, aprxTchPosArr_[n].pos, aprxTchPosArr_[n].timeStamp_tms, ySum, cErr);
                } else {
                    cErr = aprxTchPosArr_[n].pos - ySum;
                    ErrNgt[j] += cErr;
                    LOGD("  [%02u] | <RlVal=%6.1f, TmStmp=%7.3f>  (apxVal=%10.5f, apxErr=%10.6f)",
                        n, aprxTchPosArr_[n].pos, aprxTchPosArr_[n].timeStamp_tms, ySum, -cErr);
                }
            }
            LOGD("(||SYNCT||) SySld: (Pln.Order: {%u}) Calc-ed aggregate approx.error: %12.6f [bias: %10.6f].",
                j, ErrPst[j] + ErrNgt[j], ErrPst[j] - ErrNgt[j]);
            calcCmpTmp = ErrPst[j] + ErrNgt[j];
            if (calcCmpTmp < minErr) {
                minErr = calcCmpTmp;
                bstPlnOrdr = j;
            }
            // Check however the special case of primary focus on error in the "tail"-elements vicinity (as it's the point of interest
            // and of output wanted to be made for. Focusing can be done through e.g.: GenErr +2*ErrOnPreLastElm +3*ErrOnLastElm)
            // NOTE: It is supposed to be pre-checked (up to twice) that we have accumFIFO of size at least 2
            ErrPreLast[j]   = FP_ABS(AprxdPointsMtx[j][accumBufSize_ - 2] - aprxTchPosArr_[accumBufSize_ - 2].pos);
            ErrLast[j]      = FP_ABS(AprxdPointsMtx[j][accumBufSize_ - 1] - aprxTchPosArr_[accumBufSize_ - 1].pos);
            calcCmpTmp += ErrPreLast[j] * WEIGHT_COEF_FOR_PRELAST_ELEM_APXERR + ErrLast[j] * WEIGHT_COEF_FOR_LAST_ELEM_APXERR;
            LOGD("(||SYNCT||) SySld: (Pln.Order: {%u}) <PreLastRlVal=%6.1f|PreLastApxVal=%10.5f>. {LastRlVal=%6.1f|LastApxVal=%10.5f} "
                "(Calc-ed approx.error: PreLastPoint:%f|LastPoint:%f).", j, aprxTchPosArr_[accumBufSize_ - 2].pos, AprxdPointsMtx[j][accumBufSize_ - 2],
                aprxTchPosArr_[accumBufSize_ - 1].pos, AprxdPointsMtx[j][accumBufSize_ - 1], ErrPreLast[j], ErrLast[j]);
            if (calcCmpTmp < minErrEndFocused) {
                minErrEndFocused = calcCmpTmp;
                bstPlnOrdrEndFocused = j;
            }
            // TODO: Try to investigate (with the purpose of improving the approx.method precision with its predictions) the possibility of:
            //          Using track record of polynom (or its order) usage for consistency sustaining
            //          Introducing relaxation factor: the lower the order - the higher acceptance
            //              probability (for best candidate)
        }
        // Get the predicted pos value based on approximation polynoms/curves obtained.
        // Use also selected best approx polynom among of the ones of obtained orders [max-stStPwrBk .. max]
        // (if accumulated error is minimal among all tested, lower order supposed to be more smooth as well in general, perhaps :)
        // Check however the special case of primary focus on error in ending elements visinity (as it is the point of interest and output wanted)
        for(ySum =  ApxPlnCoefMtx_[bstPlnOrdrEndFocused][0], i1 = 1;  i1 <= bstPlnOrdrEndFocused;
            ySum += ApxPlnCoefMtx_[bstPlnOrdrEndFocused][i1] * FP_POW(normTimeTMs, i1), i1++);

        LOGD("(||SYNCT||) SySld: Calc-ed aggr.approx.err., using the 'top-precise' polynom of order: "
            "(GenErr:[%u] | FocErr:[%u] of maxOrder=%u), <MinGenErr=%f | MinFocErr=%f>. RSLT(Tm=%7.5f tms)= %10.5f.",
             bstPlnOrdr, bstPlnOrdrEndFocused, actAprxPlnMaxOrder, minErr, minErrEndFocused, normTimeTMs, ySum);

        return ySum;
    }

}
