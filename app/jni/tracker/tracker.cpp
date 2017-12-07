
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
        }

        void release() {
            if (isCreated) {
                mTracker1.release();
                mTracker2.release();
            }
        }

        void create() {
            mTracker1 = Tracker::create(mAlgorithm);
            mTracker2 = Tracker::create(mAlgorithm);
            isCreated = true;
        }

        void init(const Mat &image, const Rect2d &inBox1, const Rect2d &inBox2) {
            if (isCreated) {
                mTracker1->init(image, inBox1);
                mTracker2->init(image, inBox2);
            }
        }

        bool update(const Mat &image, cv::Rect2d &outBox1, cv::Rect2d &outBox2) {
            bool res = false;
            if (isCreated) {
                res = mTracker1->update(image, outBox1) && mTracker2->update(image, outBox2);
            }
            return res;
        }

        void setAlgorithm(std::string value) {
            mAlgorithm = value;
        }

        virtual ~CvTracker() {}

    protected:
        cv::Ptr<cv::Tracker> mTracker1;
        cv::Ptr<cv::Tracker> mTracker2;
        bool isCreated;
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

void JNIEXPORT JNICALL
TRACKER_JNI_METHOD(jniInit)(JNIEnv *env, jobject thisObj, jobject bitmap, jintArray rectArray) {

    cv::Rect2d obj1;
    cv::Rect2d obj2;
    cv::Mat rgbaMat;
    cv::Mat bgrMat;

    jint *rectNative = env->GetIntArrayElements(rectArray, 0);

    obj1.x = rectNative[0];
    obj1.y = rectNative[1];
    obj1.width = rectNative[2];
    obj1.height = rectNative[3];
    obj2.x = rectNative[4];
    obj2.y = rectNative[5];
    obj2.width = rectNative[6];
    obj2.height = rectNative[7];

//    LOG(INFO) << "Init tracker";

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
    trackerPtr->init(bgrMat, obj1, obj2);
    setTrackerPtr(env, thisObj, trackerPtr);
}

JNIEXPORT jobjectArray JNICALL
TRACKER_JNI_METHOD(jniDetect)(JNIEnv *env, jobject thisObj, jobject bitmap) {
    cv::Rect2d obj1;
    cv::Rect2d obj2;
    cv::Mat rgbaMat;
    cv::Mat bgrMat;
    cv::Mat grayMat;
    bool objIsFound;

    TrackerPtr trackerPtr = getTrackerPtr(env, thisObj);

    jniutils::ConvertBitmapToRGBAMat(env, bitmap, rgbaMat, true);
    cv::cvtColor(rgbaMat, bgrMat, cv::COLOR_RGB2BGR);
    objIsFound = trackerPtr->update(bgrMat, obj1, obj2);

    jobjectArray ret;
    jclass rectClass = env->FindClass("android/graphics/Rect");
    jmethodID constructor = env->GetMethodID(rectClass, "<init>", "(IIII)V");

    if (!objIsFound) {
//        LOG(INFO) << "Tracking object has been lost";
        ret = (jobjectArray) env->NewObjectArray(0, rectClass, JAVA_NULL);
    } else {
//        LOG(INFO) << "Tracking object has been found";
        ret = (jobjectArray) env->NewObjectArray(2, rectClass, JAVA_NULL);
//        LOG(INFO) << "Found 1 rect " << obj1.x << ", " << obj1.y << ", " << obj1.width << ", " << obj1.height;
        jobject rect1 = env->NewObject(rectClass, constructor, (int) obj1.x, (int) obj1.y,
                                       (int) (obj1.x + obj1.width),
                                       (int) (obj1.y + obj1.height));
        env->SetObjectArrayElement(ret, 0, rect1);
//        LOG(INFO) << "Found 2 rect " << obj2.x << ", " << obj2.y << ", " << obj2.width << ", " << obj2.height;
        jobject rect2 = env->NewObject(rectClass, constructor, (int) obj2.x, (int) obj2.y,
                                       (int) (obj2.x + obj2.width),
                                       (int) (obj2.y + obj2.height));
        env->SetObjectArrayElement(ret, 1, rect2);
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
