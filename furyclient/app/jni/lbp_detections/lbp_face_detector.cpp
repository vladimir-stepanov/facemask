
#include <jni_common/jni_bitmap2mat.h>
#include <jni_common/jni_utils.h>
#include <memory>
#include <mutex>
#include "lbp_face_detector.h"
#define LBP_FACE_DETECTOR "com/huawei/opencv/LbpFaceDetector"

using namespace std;
using namespace cv;

namespace {
    std::shared_ptr<JNI_LbpFaceDetector> getJNI_LbpFaceDetector(JNIEnv *env) {
        static std::once_flag sOnceInitFlag;
        static std::shared_ptr<JNI_LbpFaceDetector> sJNI_LbpFaceDetector;
        std::call_once(sOnceInitFlag, [env]() {
            sJNI_LbpFaceDetector = std::make_shared<JNI_LbpFaceDetector>(env);
        });
        return sJNI_LbpFaceDetector;
    }

    DetectorPtr const getDetectorPtr(JNIEnv *env, jobject thisObj) {
        std::lock_guard<std::mutex> lock(gLock);
        return getJNI_LbpFaceDetector(env)->getDetectorPtrFromJava(env, thisObj);
    }

// The function to set a pointer to java and delete it if newPtr is empty
    void setDetectorPtr(JNIEnv *env, jobject thisObj, DetectorPtr newPtr) {
        std::lock_guard<std::mutex> lock(gLock);
        DetectorPtr oldPtr = getJNI_LbpFaceDetector(env)->getDetectorPtrFromJava(env, thisObj);
        if (oldPtr != JAVA_NULL) {
            delete oldPtr;
        }

        getJNI_LbpFaceDetector(env)->setDetectorPtrToJava(env, thisObj, (jlong) newPtr);
    }

}  // end unnamed space

#ifdef __cplusplus
extern "C" {
#endif

#define LBP_FACE_JNI_METHOD(METHOD_NAME) \
  Java_com_huawei_opencv_LbpFaceDetector_##METHOD_NAME

void JNIEXPORT
LBP_FACE_JNI_METHOD(jniNativeClassInit)(JNIEnv *env, jclass _this) {}

/* return current time in milliseconds */
static long getSystemTime(void) {
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    return (long) (1000.0 * res.tv_sec + (double) res.tv_nsec / 1e6);
}

JNIEXPORT jobjectArray JNICALL
LBP_FACE_JNI_METHOD(jniBitmapDetect)(JNIEnv *env, jobject thisObj, jobject bitmap) {
    cv::Mat rgbaMat;
    cv::Mat bgrMat;
    long start = getSystemTime();
    jniutils::ConvertBitmapToRGBAMat(env, bitmap, rgbaMat, true);
    cv::cvtColor(rgbaMat, bgrMat, cv::COLOR_RGB2GRAY);
    DetectorPtr detPtr = getDetectorPtr(env, thisObj);
    vector<Rect> faces;
    detPtr->detect(bgrMat, faces);

    long end = getSystemTime();
    getJNI_LbpFaceDetector(env)->setSpentTime(env, thisObj, end - start);

    jobjectArray ret;
    jclass rectClass = env->FindClass("android/graphics/Rect");
    jmethodID constructor = env->GetMethodID(rectClass, "<init>", "(IIII)V");
    ret = (jobjectArray) env->NewObjectArray((jsize) faces.size(), rectClass, JAVA_NULL);

    for (int i = 0; i < faces.size(); i++) {
        Rect face = faces.at((u_long) i);
        jobject rect = env->NewObject(rectClass, constructor, face.x, face.y, face.x + face.width,
                                      face.y + face.height);
        env->SetObjectArrayElement(ret, i, rect);
    }
    return ret;
}

jint JNIEXPORT JNICALL
LBP_FACE_JNI_METHOD(jniInit)(JNIEnv *env, jobject thisObj, jstring jCascadeFrontalFilename) {
    std::string cascadePath = jniutils::convertJStrToString(env, jCascadeFrontalFilename);
    cv::Ptr<cv::CascadeClassifier> cascade = makePtr<cv::CascadeClassifier>(cascadePath);
    cv::Ptr<DetectionBasedTracker::IDetector> MainDetector = makePtr<CascadeDetectorAdapter>(
            cascade);
    cascade = makePtr<cv::CascadeClassifier>(cascadePath);
    cv::Ptr<DetectionBasedTracker::IDetector> TrackingDetector = makePtr<CascadeDetectorAdapter>(
            cascade);
    DetectionBasedTracker::Parameters params;
    DetectorPtr detPtr = new LbpDetector(MainDetector, TrackingDetector, params);
    detPtr->setMinObjectSize(Size(20, 20));
    detPtr->setMinNeighbours(1);
    setDetectorPtr(env, thisObj, detPtr);
    return JNI_OK;
}

jint JNIEXPORT JNICALL
LBP_FACE_JNI_METHOD(jniDeInit)(JNIEnv *env, jobject thisObj) {
    DetectorPtr detPtr = getDetectorPtr(env, thisObj);
    detPtr->stop();
    setDetectorPtr(env, thisObj, JAVA_NULL);
    return JNI_OK;
}

#ifdef __cplusplus
}
#endif
