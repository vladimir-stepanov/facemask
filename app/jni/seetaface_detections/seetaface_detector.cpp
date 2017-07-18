#include <jni_common/jni_bitmap2mat.h>
#include <jni_common/jni_utils.h>
#include <jni_common/jni_primitives.h>
#include <memory>
#include <mutex>
#include <face_detection.h>
#include <common.h>

#include "face_alignment.h"

#define SEETAFACE_DETECTOR "com/huawei/seetaface/SeetafaceDetector"

using namespace std;
using namespace seeta;
extern JNI_DetectedFace *g_pJNI_DetectedFace;

namespace {

#define JAVA_NULL 0

    class SeetafaceDetector {
    public:
        SeetafaceDetector(const char *path, const char *path2) {
            pLandmarks_detector = new seeta::FaceAlignment(path);
            pFaceBox_detector = new seeta::FaceDetection(path2);
            pFaceBox_detector->SetMinFaceSize(40);
            pFaceBox_detector->SetScoreThresh(2.f);
            pFaceBox_detector->SetImagePyramidScaleFactor(0.8f);
            pFaceBox_detector->SetWindowStep(4, 4);

        }

        std::vector<seeta::FaceInfo> getFaces(seeta::ImageData &image_data) {
            return pFaceBox_detector->Detect(image_data);
        }

        bool detect(seeta::ImageData &image_data, seeta::FaceInfo &face_bbox,
                    seeta::FacialLandmark *points) {
            return pLandmarks_detector->PointDetectLandmarks(image_data, face_bbox, points);
        }

        void stop() {
        }

        virtual ~SeetafaceDetector() {
            delete pLandmarks_detector;
            delete pFaceBox_detector;
        }

    protected:
        seeta::FaceAlignment *pLandmarks_detector;
        seeta::FaceDetection *pFaceBox_detector;
    };

    using DetectorPtr = SeetafaceDetector *;

    class JNI_SeetafaceDetector {
    public:
        JNI_SeetafaceDetector(JNIEnv *env) {
            jclass clazz = env->FindClass(SEETAFACE_DETECTOR);
            mNativeContext = env->GetFieldID(clazz, "mNativeFaceDetectorContext", "J");
            mSpentTime = env->GetFieldID(clazz, "mSpentTime", "J");
            mDetectLandmarks = env->GetFieldID(clazz, "mDetectLandmarks", "J");
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

        bool getDetectLandmarksFlag(JNIEnv *env, jobject thisObj) {
            return (bool) env->GetLongField(thisObj, mDetectLandmarks);
        }

        jfieldID mNativeContext;
        jfieldID mSpentTime;
        jfieldID mDetectLandmarks;
    };

// Protect getting/setting and creating/deleting pointer between java/native
    std::mutex gLock;

    std::shared_ptr<JNI_SeetafaceDetector> getJNI_SeetafaceDetector(JNIEnv *env) {
        static std::once_flag sOnceInitFlag;
        static std::shared_ptr<JNI_SeetafaceDetector> sJNI_SeetafaceDetector;
        std::call_once(sOnceInitFlag, [env]() {
            sJNI_SeetafaceDetector = std::make_shared<JNI_SeetafaceDetector>(env);
        });
        return sJNI_SeetafaceDetector;
    }

    DetectorPtr const getDetectorPtr(JNIEnv *env, jobject thisObj) {
        std::lock_guard<std::mutex> lock(gLock);
        return getJNI_SeetafaceDetector(env)->getDetectorPtrFromJava(env, thisObj);
    }

// The function to set a pointer to java and delete it if newPtr is empty
    void setDetectorPtr(JNIEnv *env, jobject thisObj, DetectorPtr newPtr) {
        std::lock_guard<std::mutex> lock(gLock);
        DetectorPtr oldPtr = getJNI_SeetafaceDetector(env)->getDetectorPtrFromJava(env, thisObj);
        if (oldPtr != JAVA_NULL) {
            delete oldPtr;
        }

        getJNI_SeetafaceDetector(env)->setDetectorPtrToJava(env, thisObj, (jlong) newPtr);
    }

}  // end unnamed space

#ifdef __cplusplus
extern "C" {
#endif

#define SEETAFACE_JNI_METHOD(METHOD_NAME) \
  Java_com_huawei_seetaface_SeetafaceDetector_##METHOD_NAME

void JNIEXPORT
SEETAFACE_JNI_METHOD(jniNativeClassInit)(JNIEnv *env, jclass _this) { }

/* return current time in milliseconds */
static long getSystemTime(void) {
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    return (long) (1000.0 * res.tv_sec + (double) res.tv_nsec / 1e6);
}

JNIEXPORT jobject JNICALL
SEETAFACE_JNI_METHOD(jniBitmapDetect)(JNIEnv *env, jobject thisObj, jobject bitmap) {
    cv::Mat rgbaMat;
    cv::Mat greyMat;
    long start = getSystemTime();
    jniutils::ConvertBitmapToRGBAMat(env, bitmap, rgbaMat, true);

    cv::cvtColor(rgbaMat, greyMat, cv::COLOR_RGB2GRAY);
    DetectorPtr detPtr = getDetectorPtr(env, thisObj);

    seeta::ImageData image_data(greyMat.cols, greyMat.rows);
    image_data.data = greyMat.data;
    image_data.num_channels = 1;
    const int SIZE = 5;
    seeta::FacialLandmark points[SIZE] = {{0}};

    std::vector<seeta::FaceInfo> faces = detPtr->getFaces(image_data);
    int32_t face_num = static_cast<int32_t>(faces.size());

    float angles[3];
    float modelView[16];
    float frustumScale[3];
    jobjectArray jDetRetArray = JNI_DetectedFace::createJObjectArray(env, face_num);
    for (int32_t i = 0; i < face_num; i++) {
        jobject jDetRet = JNI_DetectedFace::createJObject(env);
        env->SetObjectArrayElement(jDetRetArray, i, jDetRet);

        seeta::FaceInfo &face_bbox = faces[i];
        g_pJNI_DetectedFace->setRect(env, jDetRet,
                                     (int) face_bbox.bbox.x,
                                     (int) face_bbox.bbox.y,
                                     (int) face_bbox.bbox.x + face_bbox.bbox.width,
                                     (int) face_bbox.bbox.y + face_bbox.bbox.height);
        if (getJNI_SeetafaceDetector(env)->getDetectLandmarksFlag(env, thisObj) &&
                detPtr->detect(image_data, face_bbox, points)) {
            for (int j = 0; j < SIZE; j++) {
                int x = (int) points[j].x;
                int y = (int) points[j].y;
                // Call addLandmark
                g_pJNI_DetectedFace->addLandmark(env, jDetRet, x, y);
            }

            // 3D model points.
            std::vector<cv::Point3d> model_points;
            // Nose tip
            model_points.push_back(cv::Point3d(0.0f, 0.0f, 0.0f));
            // Chin
            model_points.push_back(cv::Point3d(0.0f, -330.0f, -65.0f));
            // Left eye left corner
            model_points.push_back(cv::Point3d(-225.0f * 0.65f, 170.0f, -135.0f));
            // Right eye right corner
            model_points.push_back(cv::Point3d(225.0f * 0.65f, 170.0f, -135.0f));
            // Left Mouth corner
            model_points.push_back(cv::Point3d(-150.0f, -150.0f, -125.0f));
            // Right mouth corner
            model_points.push_back(cv::Point3d(150.0f, -150.0f, -125.0f));

            // Camera Approximate
            double focal_length = image_data.width; // Approximate focal length.
            cv::Point2d center = cv::Point2d(image_data.width / 2, image_data.height / 2);
            cv::Mat camera_matrix = (cv::Mat_<double>(3, 3) << focal_length, 0, center.x,
                    0, focal_length, center.y, 0, 0, 1);
            // Assuming no lens distortion
            cv::Mat dist_coeffs = cv::Mat::zeros(4, 1, cv::DataType<double>::type);

            // 2D image points. If you change the image, you need to change vector
            // Y at image top is 0 (bottom is image height)
            // Numbers coming from landmark detection model
            seeta::FacialLandmark& noseTip = points[2];

            seeta::FacialLandmark chin;
            chin.x = (points[3].x + points[4].x) / 2;
            chin.y = (points[3].y + points[4].y) / 2 + points[4].x - points[3].x;
            seeta::FacialLandmark& leftEyeLeftCorner = points[0];
            seeta::FacialLandmark& rightEyeRightCorner = points[1];
            seeta::FacialLandmark& leftMouthCorner = points[3];
            seeta::FacialLandmark& rightMouthCorner = points[4];

            std::vector<cv::Point2d> image_points;
            // Nose tip
            image_points.push_back(cv::Point2d(noseTip.x, noseTip.y));
            // Chin
            image_points.push_back(cv::Point2d(chin.x, chin.y));
            // Left eye left corner
            image_points.push_back(cv::Point2d(leftEyeLeftCorner.x, leftEyeLeftCorner.y));
            // Right eye right corner
            image_points.push_back(cv::Point2d(rightEyeRightCorner.x, rightEyeRightCorner.y));
            // Left Mouth corner
            image_points.push_back(cv::Point2d(leftMouthCorner.x, leftMouthCorner.y));
            // Right mouth corner
            image_points.push_back(cv::Point2d(rightMouthCorner.x, rightMouthCorner.y));

            // Output rotation and translation
            static cv::Mat rotation_vector;
            static cv::Mat translation_vector;
            cv::solvePnPRansac(model_points, image_points, camera_matrix, dist_coeffs,
                               rotation_vector,
                               translation_vector);
            jclass jcClass = env->GetObjectClass(jDetRet);
            jfieldID anglesFId = env->GetFieldID(jcClass, "mAngles", "[F");
            jobject anglesObj = env->GetObjectField(jDetRet, anglesFId);
            jfloatArray *anglesArray = reinterpret_cast<jfloatArray *>(&anglesObj);

            jfieldID modelViewFId = env->GetFieldID(jcClass, "mModelView", "[F");
            jobject modelViewObj = env->GetObjectField(jDetRet, modelViewFId);
            jfloatArray *modelViewArray = reinterpret_cast<jfloatArray *>(&modelViewObj);

            jfieldID frustumScaleFId = env->GetFieldID(jcClass, "mFrustumScale", "[F");
            jobject frustumScaleObj = env->GetObjectField(jDetRet, frustumScaleFId);
            jfloatArray *frustumScaleArray = reinterpret_cast<jfloatArray *>(&frustumScaleObj);

            //jfieldID landmarksFId = env->GetFieldID(jcClass, "mLandmarks", "[F");
            //jobject landmarksObj = env->GetObjectField(jDetRet, landmarksFId);
            //jfloatArray *landmarksArray = reinterpret_cast<jfloatArray *>(&landmarksObj);

            // Rodrigues rotation vector
            angles[0] = (float) rotation_vector.at<double>(0);
            angles[1] = (float) rotation_vector.at<double>(1);
            angles[2] = (float) rotation_vector.at<double>(2);
            env->SetFloatArrayRegion(*anglesArray, 0, 3, angles);

            cv::Mat rotation_matrix;
            cv::Rodrigues(rotation_vector, rotation_matrix);
            memset(modelView, 0, sizeof(modelView));
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    modelView[j * 4 + k] = (float) rotation_matrix.at<double>(j, k);
                }
            }

            // Flip Y and Z-rows
            modelView[4] = -modelView[4];
            modelView[5] = -modelView[5];
            modelView[6] = -modelView[6];

            modelView[8] = -modelView[8];
            modelView[9] = -modelView[9];
            modelView[10] = -modelView[10];
            modelView[15] = 1.0f;
            env->SetFloatArrayRegion(*modelViewArray, 0, 16, modelView);

            frustumScale[0] = (float) translation_vector.at<double>(0);
            frustumScale[1] = (float) -translation_vector.at<double>(1); // Flip Y and Z
            frustumScale[2] = (float) -translation_vector.at<double>(2);
            env->SetFloatArrayRegion(*frustumScaleArray, 0, 3, frustumScale);

            std::vector<cv::Point3d> nose_end_point3D;
            std::vector<cv::Point2d> nose_end_point2D;
            nose_end_point3D.push_back(cv::Point3d(0, 0, 1000.0));
            cv::projectPoints(nose_end_point3D, rotation_vector, translation_vector, camera_matrix,
                              dist_coeffs, nose_end_point2D);

            g_pJNI_DetectedFace->setNosePoint(env, jDetRet,
                                              (int) nose_end_point2D.data()->x,
                                              (int) nose_end_point2D.data()->y);
        }
    }
    getJNI_SeetafaceDetector(env)->setSpentTime(env, thisObj, getSystemTime() - start);
    return jDetRetArray;
}

jint JNIEXPORT JNICALL
SEETAFACE_JNI_METHOD(jniInit)(JNIEnv *env, jobject thisObj, jstring jFileName1,
                              jstring jFileName2) {
    std::string cascadePath = jniutils::convertJStrToString(env, jFileName1);
    std::string cascadePath2 = jniutils::convertJStrToString(env, jFileName2);
    DetectorPtr detPtr = new SeetafaceDetector(cascadePath.c_str(), cascadePath2.c_str());
    setDetectorPtr(env, thisObj, detPtr);;
    return JNI_OK;
}

jint JNIEXPORT JNICALL
SEETAFACE_JNI_METHOD(jniDeInit)(JNIEnv *env, jobject thisObj) {
    DetectorPtr detPtr = getDetectorPtr(env, thisObj);
    detPtr->stop();
    setDetectorPtr(env, thisObj, JAVA_NULL);
    return JNI_OK;
}

#ifdef __cplusplus
}
#endif
