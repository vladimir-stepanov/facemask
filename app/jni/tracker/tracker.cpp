
#include <jni_common/jni_utils.h>
#include <jni_common/jni_bitmap2mat.h>
#include <jni.h>
#include <memory>
#include <mutex>
#include <opencv2/tracking.hpp>

using namespace std;
using namespace cv;

namespace {

#define JAVA_NULL 0
#define OBJ_TRACKER "com/hfs/furyclient/opencv/HFSObjTracker"

    class CvTracker {
    public:
        CvTracker() {
            isCreated = false;
            initialBoxIsRequired = true;
        }

        void release() {
            if (isCreated) {
                mTracker.release();
            }
        }

        void create() {
            mTracker = Tracker::create(mAlgorithm);
            initialBoxIsRequired = true;
            isCreated = true;
        }

        void init(const Mat &image, const Rect2d &inBox) {
            if (isCreated) {
                if (mTracker->init(image, inBox)) {
                    initialBoxIsRequired = false;
                }
            }
        }

        bool update(const Mat &image, cv::Rect2d &outBox) {
            bool res = false;
            if (isCreated) {
                res = mTracker->update(image, outBox);
            }
            return res;
        }

        void setInitialBoxIsRequired() {
            initialBoxIsRequired = true;
        }

        bool getInitialBoxIsRequired() {
            return initialBoxIsRequired;
        }

        void setAlgorithm(std::string value) {
            mAlgorithm = value;
        }

        virtual ~CvTracker() {}

    protected:
        cv::Ptr<cv::Tracker> mTracker;
        bool isCreated;
        bool initialBoxIsRequired;
        std::string mAlgorithm;
    };

    using TrackerPtr = CvTracker *;

    class JNI_ObjTracker {
    public:
        JNI_ObjTracker(JNIEnv *env) {
            jclass clazz = env->FindClass(OBJ_TRACKER);
            mNativeObjTrackerContext = env->GetFieldID(clazz, "mNativeObjTrackerContext", "J");
            env->DeleteLocalRef(clazz);
        }

        TrackerPtr getTrackerPtrFromJava(JNIEnv *env, jobject thisObj) {
            TrackerPtr const p = (TrackerPtr) env->GetLongField(thisObj, mNativeObjTrackerContext);
            return p;
        }

        void setTrackerPtrToJava(JNIEnv *env, jobject thisObj, jlong ptr) {
            env->SetLongField(thisObj, mNativeObjTrackerContext, ptr);
        }

        jfieldID mNativeObjTrackerContext;
    };

// Protect getting/setting and creating/deleting pointer between java/native
    std::mutex lockMutex;

    std::shared_ptr<JNI_ObjTracker> getJNI_ObjTracker(JNIEnv *env) {
        static std::once_flag sOnceInitFlag;
        static std::shared_ptr<JNI_ObjTracker> sJNI_ObjTracker;
        std::call_once(sOnceInitFlag, [env]() {
            sJNI_ObjTracker = std::make_shared<JNI_ObjTracker>(env);
        });
        return sJNI_ObjTracker;
    }

    TrackerPtr const getTrackerPtr(JNIEnv *env, jobject thisObj) {
        std::lock_guard<std::mutex> lock(lockMutex);
        return getJNI_ObjTracker(env)->getTrackerPtrFromJava(env, thisObj);
    }

// The function to set a pointer to java and delete it if newPtr is empty
    void setTrackerPtr(JNIEnv *env, jobject thisObj, TrackerPtr newPtr) {
        std::lock_guard<std::mutex> lock(lockMutex);
        TrackerPtr oldPtr = getJNI_ObjTracker(env)->getTrackerPtrFromJava(env, thisObj);
        if (newPtr == JAVA_NULL) {
            oldPtr->release();
            delete oldPtr;
        }

        getJNI_ObjTracker(env)->setTrackerPtrToJava(env, thisObj, (jlong) newPtr);
    }

}  // end unnamed space

#ifdef __cplusplus
extern "C" {
#endif

#define TRACKER_JNI_METHOD(METHOD_NAME) \
  Java_com_hfs_furyclient_opencv_HFSObjTracker_##METHOD_NAME

void JNIEXPORT
TRACKER_JNI_METHOD(jniNativeClassInit)(JNIEnv *env, jclass _this) {}

jint JNIEXPORT JNICALL
TRACKER_JNI_METHOD(jniInit)(JNIEnv *env, jobject thisObj, jobject bitmap) {

    cv::Rect2d obj;
    cv::Mat rgbaMat;
    cv::Mat bgrMat;

    obj.x = 40;
    obj.y = 40;
    obj.width = 20;
    obj.height = 20;

    LOG(INFO) << "Init tracker";

    jniutils::ConvertBitmapToRGBAMat(env, bitmap, rgbaMat, true);
    cv::cvtColor(rgbaMat, bgrMat, cv::COLOR_RGB2BGR);

    //init tracker
    TrackerPtr trackerPtr = getTrackerPtr(env, thisObj);
    if (trackerPtr) {
        trackerPtr->release();
    }
    trackerPtr = new CvTracker();
    trackerPtr->setAlgorithm("MEDIANFLOW");
    trackerPtr->create();
    trackerPtr->init(bgrMat, obj);
    setTrackerPtr(env, thisObj, trackerPtr);

    return JNI_OK;
}

JNIEXPORT jobjectArray JNICALL
TRACKER_JNI_METHOD(jniDetect)(JNIEnv *env, jobject thisObj, jobject bitmap) {
    cv::Rect2d obj;
    cv::Mat rgbaMat;
    cv::Mat bgrMat;
    cv::Mat grayMat;
    bool objIsFound;

    TrackerPtr trackerPtr = getTrackerPtr(env, thisObj);

    jniutils::ConvertBitmapToRGBAMat(env, bitmap, rgbaMat, true);
    cv::cvtColor(rgbaMat, bgrMat, cv::COLOR_RGB2BGR);
    objIsFound = trackerPtr->update(bgrMat, obj);

    if (!objIsFound) {
        LOG(INFO) << "Tracking object has been lost";
    } else {
        LOG(INFO) << "Tracking object has been found";
    }

    jobjectArray ret;
    jclass rectClass = env->FindClass("android/graphics/Rect");
    jmethodID constructor = env->GetMethodID(rectClass, "<init>", "(IIII)V");

    if (obj.width > 0 && obj.height > 0) {
        ret = (jobjectArray) env->NewObjectArray(1, rectClass, JAVA_NULL);

        jobject rect = env->NewObject(rectClass, constructor, (int) obj.x, (int) obj.y,
                                      (int) (obj.x + obj.width),
                                      (int) (obj.y + obj.height));
        env->SetObjectArrayElement(ret, 0, rect);
    } else {
        ret = (jobjectArray) env->NewObjectArray(0, rectClass, JAVA_NULL);
    }
    return ret;
}

jint JNIEXPORT JNICALL
TRACKER_JNI_METHOD(jniDel)(JNIEnv *env, jobject thisObj) {
    setTrackerPtr(env, thisObj, JAVA_NULL);
    return JNI_OK;
}

#ifdef __cplusplus
}
#endif
