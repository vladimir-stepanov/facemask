
#include <jni_common/jni_bitmap2mat.h>
#include <jni_common/jni_utils.h>
#include <memory>
#include <mutex>
#include "haar_detector.h"

using namespace std;
using namespace cv;

namespace {
    std::shared_ptr<JNI_HaarDetector> getJNI_HaarDetector(JNIEnv *env) {
        static std::once_flag sOnceInitFlag;
        static std::shared_ptr<JNI_HaarDetector> sJNI_HaarDetector;
        std::call_once(sOnceInitFlag, [env]() {
            sJNI_HaarDetector = std::make_shared<JNI_HaarDetector>(env);
        });
        return sJNI_HaarDetector;
    }

    DetectorPtr const getDetectorPtr(JNIEnv *env, jobject thisObj) {
        std::lock_guard<std::mutex> lock(gLock);
        return getJNI_HaarDetector(env)->getDetectorPtrFromJava(env, thisObj);
    }

// The function to set a pointer to java and delete it if newPtr is empty
    void setDetectorPtr(JNIEnv *env, jobject thisObj, DetectorPtr newPtr) {
        std::lock_guard<std::mutex> lock(gLock);
        DetectorPtr oldPtr = getJNI_HaarDetector(env)->getDetectorPtrFromJava(env, thisObj);
        if (oldPtr != JAVA_NULL) {
            delete oldPtr;
        }

        getJNI_HaarDetector(env)->setDetectorPtrToJava(env, thisObj, (jlong) newPtr);
    }

}  // end unnamed space

#ifdef __cplusplus
extern "C" {
#endif

#define HAAR_JNI_METHOD(METHOD_NAME) \
  Java_com_huawei_opencv_HaarDetector_##METHOD_NAME

void JNIEXPORT
HAAR_JNI_METHOD(jniNativeClassInit)(JNIEnv *env, jclass _this) {}

/* return current time in milliseconds */
static long getSystemTime(void) {
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    return (long) (1000.0 * res.tv_sec + (double) res.tv_nsec / 1e6);
}

JNIEXPORT jobjectArray JNICALL
HAAR_JNI_METHOD(jniBitmapDetect)(JNIEnv *env, jobject thisObj, jobject bitmap) {
    cv::Mat rgbaMat;
    cv::Mat bgrMat;
    long start = getSystemTime();
    jniutils::ConvertBitmapToRGBAMat(env, bitmap, rgbaMat, true);
    cv::cvtColor(rgbaMat, bgrMat, cv::COLOR_RGB2GRAY);
    DetectorPtr detPtr = getDetectorPtr(env, thisObj);
    vector<Rect> objs;
    detPtr->detect(bgrMat, objs);

    long end = getSystemTime();
    getJNI_HaarDetector(env)->setSpentTime(env, thisObj, end - start);

    jobjectArray ret;
    jclass rectClass = env->FindClass("android/graphics/Rect");
    jmethodID constructor = env->GetMethodID(rectClass, "<init>", "(IIII)V");
    ret = (jobjectArray) env->NewObjectArray((jsize) objs.size(), rectClass, JAVA_NULL);

    for (int i = 0; i < objs.size(); i++) {
        Rect obj = objs.at((u_long) i);
        jobject rect = env->NewObject(rectClass, constructor, obj.x, obj.y, obj.x + obj.width,
                                      obj.y + obj.height);
        env->SetObjectArrayElement(ret, i, rect);
    }
    return ret;
}

jint JNIEXPORT JNICALL
HAAR_JNI_METHOD(jniInit)(JNIEnv *env, jobject thisObj, jstring jCascadeFrontalFilename) {
    std::string cascadePath = jniutils::convertJStrToString(env, jCascadeFrontalFilename);
    cv::Ptr<cv::CascadeClassifier> cascade = makePtr<cv::CascadeClassifier>(cascadePath);
    cv::Ptr<DetectionBasedTracker::IDetector> MainDetector = makePtr<CascadeDetectorAdapter>(
            cascade);
    cascade = makePtr<cv::CascadeClassifier>(cascadePath);
    cv::Ptr<DetectionBasedTracker::IDetector> TrackingDetector = makePtr<CascadeDetectorAdapter>(
            cascade);
    DetectionBasedTracker::Parameters params;
    DetectorPtr detPtr = new HaarDetector(MainDetector, TrackingDetector, params);
    detPtr->setMinObjectSize(Size(20, 20));
    detPtr->setMinNeighbours(1);
    setDetectorPtr(env, thisObj, detPtr);;
    return JNI_OK;
}

jint JNIEXPORT JNICALL
HAAR_JNI_METHOD(jniDeInit)(JNIEnv *env, jobject thisObj) {
    DetectorPtr detPtr = getDetectorPtr(env, thisObj);
    detPtr->stop();
    setDetectorPtr(env, thisObj, JAVA_NULL);
    return JNI_OK;
}

#ifdef __cplusplus
}
#endif
