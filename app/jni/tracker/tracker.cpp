
#include <jni_common/jni_bitmap2mat.h>
#include <jni_common/jni_utils.h>
#include <memory>
#include <mutex>
#include <opencv2/tracking.hpp>
#include <lbp_detections/lbp_face_detector.h>

using namespace std;
using namespace cv;

namespace {

#define JAVA_NULL 0
#define OBJ_TRACKER "com/huawei/opencv/ObjTracker"
#define DEFAULT_ALGORITHM "MIL"

    class CvTracker {
    public:
        CvTracker() {
            isCreated = false;
            initialBoxIsRequired = true;
            mAlgorithm = DEFAULT_ALGORITHM;
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

        bool getInitialBoxIsRequired() {
            return initialBoxIsRequired;
        }

        std::string getAlgorithm() {
            return mAlgorithm;
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
    using DetectorPtr = LbpDetector *;

    class JNI_ObjTracker {
    public:
        JNI_ObjTracker(JNIEnv *env) {
            jclass clazz = env->FindClass(OBJ_TRACKER);
            mNativeFaceTrackerContext = env->GetFieldID(clazz, "mNativeFaceTrackerContext", "J");
            mNativeFaceDetectorContext = env->GetFieldID(clazz, "mNativeFaceDetectorContext", "J");
            mSpentTime = env->GetFieldID(clazz, "mSpentTime", "J");
            env->DeleteLocalRef(clazz);
        }

        TrackerPtr getTrackerPtrFromJava(JNIEnv *env, jobject thisObj) {
            TrackerPtr const p = (TrackerPtr) env->GetLongField(thisObj, mNativeFaceTrackerContext);
            return p;
        }

        void setTrackerPtrToJava(JNIEnv *env, jobject thisObj, jlong ptr) {
            env->SetLongField(thisObj, mNativeFaceTrackerContext, ptr);
        }

        DetectorPtr getDetectorPtrFromJava(JNIEnv *env, jobject thisObj) {
            DetectorPtr const p = (DetectorPtr) env->GetLongField(thisObj,
                                                                  mNativeFaceDetectorContext);
            return p;
        }

        void setDetectorPtrToJava(JNIEnv *env, jobject thisObj, jlong ptr) {
            env->SetLongField(thisObj, mNativeFaceDetectorContext, ptr);
        }

        void setSpentTime(JNIEnv *env, jobject thisObj, jlong ptr) {
            env->SetLongField(thisObj, mSpentTime, ptr);
        }

        jfieldID mNativeFaceTrackerContext;
        jfieldID mNativeFaceDetectorContext;
        jfieldID mSpentTime;
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

    DetectorPtr const getDetectorPtr(JNIEnv *env, jobject thisObj) {
        std::lock_guard<std::mutex> lock(lockMutex);
        return getJNI_ObjTracker(env)->getDetectorPtrFromJava(env, thisObj);
    }

// The function to set a pointer to java and delete it if newPtr is empty
    void setDetectorPtr(JNIEnv *env, jobject thisObj, DetectorPtr newPtr) {
        std::lock_guard<std::mutex> lock(lockMutex);
        DetectorPtr oldPtr = getJNI_ObjTracker(env)->getDetectorPtrFromJava(env, thisObj);
        if (oldPtr != JAVA_NULL) {
            delete oldPtr;
        }

        getJNI_ObjTracker(env)->setDetectorPtrToJava(env, thisObj, (jlong) newPtr);
    }
}  // end unnamed space


#ifdef __cplusplus
extern "C" {
#endif

#define TRACKER_JNI_METHOD(METHOD_NAME) \
  Java_com_huawei_opencv_ObjTracker_##METHOD_NAME

void JNIEXPORT
TRACKER_JNI_METHOD(jniNativeClassInit)(JNIEnv *env, jclass _this) {}

/* return current time in milliseconds */
static long getSystemTime(void) {
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    return (long) (1000.0 * res.tv_sec + (double) res.tv_nsec / 1e6);
}

jint JNIEXPORT JNICALL
TRACKER_JNI_METHOD(jniInit)(JNIEnv *env, jobject thisObj, jstring jCascadePath) {
    //detector
    std::string cascadePath = jniutils::convertJStrToString(env, jCascadePath);
    cv::Ptr<cv::CascadeClassifier> cascade = makePtr<cv::CascadeClassifier>(cascadePath);
    cv::Ptr<DetectionBasedTracker::IDetector> MainDetector = makePtr<CascadeDetectorAdapter>(
            cascade);
    cascade = makePtr<cv::CascadeClassifier>(cascadePath);
    cv::Ptr<DetectionBasedTracker::IDetector> TrackingDetector = makePtr<CascadeDetectorAdapter>(
            cascade);
    DetectionBasedTracker::Parameters params;
    DetectorPtr detectorPtr = new LbpDetector(MainDetector, TrackingDetector, params);
    detectorPtr->setMinObjectSize(Size(20, 20));
    detectorPtr->setMinNeighbours(1);
    setDetectorPtr(env, thisObj, detectorPtr);

    //tracker
    TrackerPtr trackerPtr = new CvTracker();
    trackerPtr->create();
    setTrackerPtr(env, thisObj, trackerPtr);

    return JNI_OK;
}

JNIEXPORT jobjectArray JNICALL
TRACKER_JNI_METHOD(jniDetect)(JNIEnv *env, jobject thisObj, jobject bitmap, jstring jAlgorithm) {
    cv::Rect2d face;
    cv::Mat rgbaMat;
    cv::Mat bgrMat;
    cv::Mat grayMat;
    bool needSaveTracker = false;
    bool faceIsFound = false;
    bool trackerIsActive = false;

    long start = getSystemTime();

    std::string algorithm = jniutils::convertJStrToString(env, jAlgorithm);
    TrackerPtr trackerPtr = getTrackerPtr(env, thisObj);

    std::string currentAlgorithm = trackerPtr->getAlgorithm();
    if (currentAlgorithm.compare(algorithm) != 0) {
        trackerPtr->release();
        trackerPtr->setAlgorithm(algorithm);
        trackerPtr->create();
        needSaveTracker = true;
    }

    jniutils::ConvertBitmapToRGBAMat(env, bitmap, rgbaMat, true);
    if (trackerPtr->getInitialBoxIsRequired()) {
        DetectorPtr detectorPtr = getDetectorPtr(env, thisObj);
        vector<cv::Rect> faces;

        cv::cvtColor(rgbaMat, grayMat, cv::COLOR_RGB2GRAY);
        detectorPtr->detect(grayMat, faces);

        if (faces.size()) {
            cv::Rect rect = faces.at(0);
            face.x = (double) rect.x;
            face.y = (double) rect.y;
            face.width = (double) rect.width;
            face.height = (double) rect.width;

            if (face.width > 0 && face.height > 0 && face.x >= 0 && face.y >= 0) {
                cv::cvtColor(rgbaMat, bgrMat, cv::COLOR_RGB2BGR);
                trackerPtr->init(bgrMat, face);
                needSaveTracker = true;
                faceIsFound = true;
            }
        }
    } else {
        trackerIsActive = true;
        cv::cvtColor(rgbaMat, bgrMat, cv::COLOR_RGB2BGR);
        faceIsFound = trackerPtr->update(bgrMat, face);

        if (!faceIsFound &&
                ((trackerPtr->getAlgorithm().compare("MEDIANFLOW") == 0) ||
                        (trackerPtr->getAlgorithm().compare("KCF") == 0))) {
            trackerPtr->setAlgorithm(DEFAULT_ALGORITHM);
            needSaveTracker = true;
        }

    }

    if (needSaveTracker) {
        setTrackerPtr(env, thisObj, trackerPtr);
    }

    long end = getSystemTime();
    getJNI_ObjTracker(env)->setSpentTime(env, thisObj, end - start);

//    if (trackerIsActive) {
//        LOG(INFO) << "jniDetect( " << algorithm << " ): Is TRACKER found face: " << faceIsFound;
//        if (faceIsFound) {
//            LOG(INFO)
//            << "jniDetect( " << algorithm << " ):" << " x=" << face.x << ", y =" << face.y << ", w="
//            << face.width << ", h = " << face.height;
//        }
//    } else {
//        LOG(INFO) << "jniDetect( " << algorithm << " ): Is DETECTOR found face: " << faceIsFound;
//        if (faceIsFound) {
//            LOG(INFO)
//            << "jniDetect( " << algorithm << " ):" << " x=" << face.x << ", y =" << face.y << ", w="
//            << face.width << ", h = " << face.height;
//        }
//    }

    jobjectArray ret;
    jclass rectClass = env->FindClass("android/graphics/Rect");
    jmethodID constructor = env->GetMethodID(rectClass, "<init>", "(IIII)V");

    if (face.width > 0 && face.height > 0) {
        ret = (jobjectArray) env->NewObjectArray(1, rectClass, JAVA_NULL);

        jobject rect = env->NewObject(rectClass, constructor, (int) face.x, (int) face.y,
                                      (int) (face.x + face.width),
                                      (int) (face.y + face.height));
        env->SetObjectArrayElement(ret, 0, rect);
    } else {
        ret = (jobjectArray) env->NewObjectArray(0, rectClass, JAVA_NULL);
    }
    return ret;
}

jint JNIEXPORT JNICALL
TRACKER_JNI_METHOD(jniDel)(JNIEnv *env, jobject thisObj) {
    setTrackerPtr(env, thisObj, JAVA_NULL);
    setDetectorPtr(env, thisObj, JAVA_NULL);
    return JNI_OK;
}

#ifdef __cplusplus
}
#endif
