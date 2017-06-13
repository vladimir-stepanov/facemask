/**
 * Copyright (C) 2013-2015 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SYNCED_SLIDE_EVENT
#define GRAPEVINE_SYNCED_SLIDE_EVENT

#include <stdint.h>

#define MAX_FIFO_SIZE               16
#define MAX_FIFO_SIZE_MASK          0x0F // for pwr of 2 buf size constant above^ (16)
#define MAX_APX_DATA_CNT_IN_USE     8   // Data buffer size sufficient for approximation  (Mu.B. >= MAX_POLYN_ORDER)  (?m.b. take it == MAX_FIFO_SIZE?)
#define MAX_POLYN_ORDER             4  // Enough to be smooth. N5 compute cost: ~30us (20-60us)  // Max power of highest order polynom
#define MAX_SQUARE_POLYN_ORDER      2 // Max power of highest order of "square" polynom is 2 :)

// Select internal precision of FP op-s, so the dependent defines (dft - float)
//#define FP_DOUBLE_PREC

#ifdef FP_DOUBLE_PREC
 typedef double                     fpformat;
 #define FP_ABS(A)                  fabs(A)
 #define FP_POW(a, b)               pow(a, b)
 #define MIN_SCENE_SHIFT_TO_REDRAW  .1e-2  // to be in sync with the corresponding auto-scroll const
 #define MS_TO_APROXCALC_TMS_MULT   .02
 #define FRAME_TIME_MS              16.667
 #define FRAME_TIME_US              FRAME_TIME_MS * 1000.0
 #define NS_TO_MS_MULT              .1e-5
 #define US_TO_MS_MULT              .1e-2
 #define TMS_TO_US_MULT             1.0 / (US_TO_MS_MULT * MS_TO_APROXCALC_TMS_MULT)  // 100000.0
 #define FP_HLF_MLT                 .5
#else
 typedef float                      fpformat;
 #define FP_ABS(a)                  fabsf(a)
 #define FP_POW(a, b)               powf(a, b)
 #define MIN_SCENE_SHIFT_TO_REDRAW  .1e-2f  // to be in sync with the corresponding auto-scroll const
 #define MS_TO_APROXCALC_TMS_MULT   .02f
 #define FRAME_TIME_MS              16.667f
 #define FRAME_TIME_US              FRAME_TIME_MS * 1000.0f
 #define NS_TO_MS_MULT              .1e-5f
 #define US_TO_MS_MULT              .1e-2f
 #define TMS_TO_US_MULT             1.0f / (US_TO_MS_MULT * MS_TO_APROXCALC_TMS_MULT)  // 100000.0f
 #define FP_HLF_MLT                 .5f
#endif

#define REAL_TIMESTAMP_DIFF_US      10 //?
#define US_TO_TMS_MULT              US_TO_MS_MULT * MS_TO_APROXCALC_TMS_MULT
#define FRAME_TIME_TMS              FRAME_TIME_MS * MS_TO_APROXCALC_TMS_MULT
#define NS_TO_TMS_MULT              NS_TO_MS_MULT * MS_TO_APROXCALC_TMS_MULT


// NOTE: On [ MS_TO_APROXCALC_TMS_MULT ] -multiplier above.
//       Due to nature of ChePolyno approximation of this kind (proposed ~.2K yrs back ;), in order to keep accumulated calculated
//       sums tidy, the ordinate value is expected to be close to 1. In our case this ordinate represents time which we were originally
//       keeping in ms. However, during long time swipe/flick this value could have easily reached tens of thousands that was badly
//       affecting approximation precision. Hence, the MS_TO_APROXCALC_TMS_MULT was introduced as a 1st step to address the problem
//       (that actually made the ordinate values order to change from msecs to parts of sec-s (kept in 'float') the in approx.math now).

namespace Grapevine
{
    /**
     * @enum apprxMode  Defines the type of approximation to be used for arranging the collected input data into smooth curve
     *                  and getting the value from that later for any feasible non-discrete timepoint
     */
    enum apprxMode {
        piecewsLinear = 0,      /**< fast piecewise linear,              is coded as uint8_t of value 0 */
        polynomial = 1,         /**< robust polynomial,                  is coded as uint8_t of value 1 */
        polynomial_sq = 2,      /**< robust polynomial (max of order 2), is coded as uint8_t of value 2 */
        combined = 3,           /**< combined of two 1st above,          is coded as uint8_t of value 3 */
        undefined = 100         /**< undefined (auto),                   is coded as uint8_t of value 100 */
    };

    /**
     * @class SyncedSlideEvent
     *
     *  Class for collecting the async touch-move events on the timeline, creating the approximating curve (e.g. Least Squares polynomial
     *  of given order) based on which taking the predicted touch-move position at any moment - in our case the FPS synced timepoints
     */
    class SyncedSlideEvent
    {
    public:
        //CLASS_UNCOPYABLE(SyncedSlideEvent);
        /**
         *  C-tor
         */
        SyncedSlideEvent();

        /**
         *  Initializes shift curve by 1st step vector
         *
         * @param [in]  touchPos             Initial (touch-down) horizontal/vertical touch-move (finger) shift position.
         * @param [in]  touchTimeEventUs     Timepoint (us) at which (touch-down) the input event was registered by the peripherial
         *                                   kernel driver for the @touchPos position.
         * @param [in]  touchEventRegTimeNs  Timepoint (ns) at which (touch-down) input event came and was registered for the
         *                                   @touchPos position in the events FIFO.
         * @param [in]  delayMsForSlidePosApprxCalc Reliable delay (ms) for calculating approximated slide position (for ensuring
         *                                   best possible smoothness and lowest lag time).
         * @param [in]  typeOfApprx          Approximation mode: default - fast piecewise 1st order linear approximation.
         */
        void InitTouchTrackDataUs(float touchPos, uint64_t touchTimeEventUs, uint64_t touchEventRegTimeNs, float delayMsForSlidePosApprxCalc,
                                apprxMode typeOfApprx = apprxMode::piecewsLinear);

        /**
         * Updates the accumulated touch-move points track vector.
         * Recalculates (updates) the approximation curve.
         *
         * @param [in]  pos     Current horizontal/vertical touch-move (finger) shift position
         * @param [in]  timeUs  Timepoint (us) at which current input event is registered for the @pos position
         * @return              Token that indicates the need to re-push touch-scroll rendering sequence stopped earlier
         *                      due to insufficient movement
         */
        bool updateTouchMoveEventsUsFifo(float pos, uint64_t timeStamp_ns);

        /**
         * Get the predicted (extrapolated) value of the touch-move curve for the givent time
         *
         * @param [in]  timeRequestedNs Timepoint (ns) at which X/Y position is needed
         *                              (assumed the FPS -synced render entrance timestamp is given)
         * @param [out] needContinueRendering Return token to indicate whether we have at least 1 pixel position change
         *                              compared to previous call, so it has sense to continue rendering
         * @return                      Predicted (extrapolated) value of the touch-move curve for the given time
         */
        float getSlidePosNowNs(uint64_t timeRequestedNs, bool& needContinueRendering);

    private:

        fpformat    getPiecewiseLinearInterpltnOut(uint64_t adjustedTimeOfReqNs);
        fpformat    getChePolynoOut(uint8_t curBufSize, uint64_t adjustedTimeOfReqNs);
        uint64_t    InitPolyBaseAndGetCurTimePedestal(uint8_t actualCollctdBufSize, uint8_t apxPlnMxPwr);
        void        UpdateChePolynoMatrix(uint8_t activeApxPlnMaxPwr);
        fpformat    UpdateAprxPolynCoefMtxAndGetPos(uint8_t actAprxPlnMaxOrder, uint64_t normalizedTimeOfReqNs);

        uint8_t arrStPtr_, arrEnPtr_, lastCIdx_, accumBufSize_;
        bool    usePiecewiseLinearApprx_, renderSequenceWasPaused_, regressionCurveGrows_;
        bool    inPredictionState_, ltdPolyPrediction_; // separate are needed due to possibility of using both alg-s at once

        struct  inputTouchPoint{
            fpformat    pos;
            uint64_t    timeStamp_us;
        };
        inputTouchPoint inTchPosArr_[MAX_FIFO_SIZE];

        struct  approxTouchPoint{
            fpformat    pos, timeStamp_tms;
        };
        approxTouchPoint aprxTchPosArr_[MAX_APX_DATA_CNT_IN_USE];

        // Chebyshev Orthogonal Polynomial ("Parabolic") Approximation
        fpformat POrChePolynoMtx_[MAX_POLYN_ORDER + 1][MAX_FIFO_SIZE + 1];
        fpformat ApxPlnCoefMtx_[MAX_POLYN_ORDER + 1][MAX_FIFO_SIZE + 1];
        fpformat PhiCntd_[MAX_POLYN_ORDER + 1][MAX_FIFO_SIZE + 1];
        fpformat Denm_[MAX_POLYN_ORDER + 1];
        uint64_t touchDownEventUs_, hugeBackTimeShiftNs_, prevPosRepRelTmStmpNs_;
        fpformat prevPosReported_, renderSequencePausedPos_;
        fpformat K_, C_, prevK_, prevC_;
        // Polynomial related (to keep it's simultaneous usage poss-ty with linear)
        fpformat lastPolySlwDwnPos_, activePolyLmt_;
        uint64_t delayNsForSlidePosApprxCalc_;
        bool    prevIsNotUltdPolyPredicted_;
        uint8_t maxAprxPolynPwr;

#ifdef DEBUG
        inputTouchPoint outDbBiFifo[MAX_FIFO_SIZE + 1];
#endif
    };

} // namespace Grpvn

#endif //SYNCED_SLIDE_EVENT