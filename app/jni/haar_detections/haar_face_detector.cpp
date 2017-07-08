
#include <jni_common/jni_bitmap2mat.h>
#include <jni_common/jni_utils.h>
#include <memory>
#include <mutex>

#define HAAR_FACE_DETECTOR "com/huawei/opencv/HaarFaceDetector"

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

        void detect(const cv::Mat &imageGray, vector<Rect> &faces) {
            if (!mIsRunning) {
                mDetector.run();
                mIsRunning = true;
            }
            mDetector.process(imageGray);
            mDetector.getObjects(faces);
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

    class JNI_HaarFaceDetector {
    public:
        JNI_HaarFaceDetector(JNIEnv *env) {
            jclass clazz = env->FindClass(HAAR_FACE_DETECTOR);
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

    std::shared_ptr<JNI_HaarFaceDetector> getJNI_HaarFaceDetector(JNIEnv *env) {
        static std::once_flag sOnceInitFlag;
        static std::shared_ptr<JNI_HaarFaceDetector> sJNI_HaarFaceDetector;
        std::call_once(sOnceInitFlag, [env]() {
            sJNI_HaarFaceDetector = std::make_shared<JNI_HaarFaceDetector>(env);
        });
        return sJNI_HaarFaceDetector;
    }

    DetectorPtr const getDetectorPtr(JNIEnv *env, jobject thisObj) {
        std::lock_guard<std::mutex> lock(gLock);
        return getJNI_HaarFaceDetector(env)->getDetectorPtrFromJava(env, thisObj);
    }

// The function to set a pointer to java and delete it if newPtr is empty
    void setDetectorPtr(JNIEnv *env, jobject thisObj, DetectorPtr newPtr) {
        std::lock_guard<std::mutex> lock(gLock);
        DetectorPtr oldPtr = getJNI_HaarFaceDetector(env)->getDetectorPtrFromJava(env, thisObj);
        if (oldPtr != JAVA_NULL) {
            delete oldPtr;
        }

        getJNI_HaarFaceDetector(env)->setDetectorPtrToJava(env, thisObj, (jlong) newPtr);
    }

}  // end unnamed space

#ifdef __cplusplus
extern "C" {
#endif

#define HAAR_FACE_JNI_METHOD(METHOD_NAME) \
  Java_com_huawei_opencv_HaarFaceDetector_##METHOD_NAME

void JNIEXPORT
HAAR_FACE_JNI_METHOD(jniNativeClassInit)(JNIEnv *env, jclass _this) {}

/* return current time in milliseconds */
static long getSystemTime(void) {
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    return (long) (1000.0 * res.tv_sec + (double) res.tv_nsec / 1e6);
}

JNIEXPORT jobjectArray JNICALL
HAAR_FACE_JNI_METHOD(jniBitmapDetect)(JNIEnv *env, jobject thisObj, jobject bitmap) {
    cv::Mat rgbaMat;
    cv::Mat bgrMat;
    long start = getSystemTime();
    jniutils::ConvertBitmapToRGBAMat(env, bitmap, rgbaMat, true);
    cv::cvtColor(rgbaMat, bgrMat, cv::COLOR_RGB2GRAY);
    DetectorPtr detPtr = getDetectorPtr(env, thisObj);
    vector<Rect> faces;
    detPtr->detect(bgrMat, faces);

    long end = getSystemTime();
    getJNI_HaarFaceDetector(env)->setSpentTime(env, thisObj, end - start);

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
HAAR_FACE_JNI_METHOD(jniInit)(JNIEnv *env, jobject thisObj, jstring jCascadeFrontalFilename) {
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
HAAR_FACE_JNI_METHOD(jniDeInit)(JNIEnv *env, jobject thisObj) {
    DetectorPtr detPtr = getDetectorPtr(env, thisObj);
    detPtr->stop();
    setDetectorPtr(env, thisObj, JAVA_NULL);
    return JNI_OK;
}

#ifdef __cplusplus
}
#endif
