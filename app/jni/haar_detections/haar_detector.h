#pragma once

#include <jni_common/jni_bitmap2mat.h>
#include <jni_common/jni_utils.h>
#include <memory>
#include <mutex>

#define HAAR_DETECTOR "com/huawei/opencv/HaarDetector"

using namespace std;
using namespace cv;

namespace {

#define JAVA_NULL 0

    class CascadeDetectorAdapter : public DetectionBasedTracker::IDetector {
    public:
        CascadeDetectorAdapter(cv::Ptr<cv::CascadeClassifier> detector) :
                IDetector(),
                Detector(detector) {
            CV_Assert(detector);
        }

        void detect(const cv::Mat &Image, std::vector<cv::Rect> &objects) {

            Detector->detectMultiScale(Image, objects, scaleFactor, minNeighbours, 0, minObjSize,
                                       maxObjSize);
        }

        virtual ~CascadeDetectorAdapter() {}

    private:
        CascadeDetectorAdapter() {
        }

        cv::Ptr<cv::CascadeClassifier> Detector;
    };

    class HaarDetector {
    public:
        HaarDetector(cv::Ptr<DetectionBasedTracker::IDetector> MainDetector,
                     cv::Ptr<DetectionBasedTracker::IDetector> TrackingDetector,
                     DetectionBasedTracker::Parameters params)
                : mDetector(MainDetector, TrackingDetector, params) {
            mMainDetector = MainDetector;
        }

        void setMinObjectSize(const cv::Size &minSize) {
            mMainDetector->setMinObjectSize(minSize);
        }

        void setMinNeighbours(int value) {
            mMainDetector->setMinNeighbours(value);
        }

        void detect(const cv::Mat &imageGray, vector<Rect> &objs) {
            if (!mIsRunning) {
                mDetector.run();
                mIsRunning = true;
            }
            mDetector.process(imageGray);
            mDetector.getObjects(objs);
        }

        void stop() {
            mDetector.stop();
            mIsRunning = false;
        }

        virtual ~HaarDetector() {}

    protected:
        DetectionBasedTracker mDetector;
        cv::Ptr<DetectionBasedTracker::IDetector> mMainDetector;
        bool mIsRunning = false;
    };

    using DetectorPtr = HaarDetector *;

    class JNI_HaarDetector {
    public:
        JNI_HaarDetector(JNIEnv *env) {
            jclass clazz = env->FindClass(HAAR_DETECTOR);
            mNativeContext = env->GetFieldID(clazz, "mNativeFaceDetectorContext", "J");
            mSpentTime = env->GetFieldID(clazz, "mSpentTime", "J");
            env->DeleteLocalRef(clazz);
        }

        DetectorPtr getDetectorPtrFromJava(JNIEnv *env, jobject thisObj) {
            DetectorPtr const p = (DetectorPtr) env->GetLongField(thisObj, mNativeContext);
            return p;
        }

        void setDetectorPtrToJava(JNIEnv *env, jobject thisObj, jlong ptr) {
            env->SetLongField(thisObj, mNativeContext, ptr);
        }

        void setSpentTime(JNIEnv *env, jobject thisObj, jlong ptr) {
            env->SetLongField(thisObj, mSpentTime, ptr);
        }

        jfieldID mNativeContext;
        jfieldID mSpentTime;

	};

// Protect getting/setting and creating/deleting pointer between java/native
    std::mutex gLock;

}  // end unnamed space

