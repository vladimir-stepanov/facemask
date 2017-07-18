
#include <android/bitmap.h>
#include <jni_common/jni_bitmap2mat.h>
#include <jni_common/jni_primitives.h>
#include <jni_common/jni_fileutils.h>
#include <jni_common/jni_utils.h>
#include <detector.h>

#include <dlib/opencv.h>
#include <lbp_detections/lbp_face_detector.h>

using namespace cv;

extern JNI_DetectedFace *g_pJNI_DetectedFace;

namespace {

#define JAVA_NULL 0
    using DetectorPtr_DLIB = DLibHOGFaceDetector *;
    using DetectorPtr2 = LbpDetector *;
    #define CLASSNAME_FACE_DETECTOR_MOD "com/huawei/dlib/DlibModFaceDetector"

    class JNI_FaceDetector {
    public:
        JNI_FaceDetector(JNIEnv *env) {
            jclass clazz = env->FindClass(CLASSNAME_FACE_DETECTOR_MOD);
            mNativeContext = env->GetFieldID(clazz, "mNativeFaceDetectorContext", "J");
            mNativeContext2 = env->GetFieldID(clazz, "mNativeFaceDetectorContext2", "J");
            mTimeSpent = env->GetFieldID(clazz, "mSpentTime", "J");
            mDetectLandmarks = env->GetFieldID(clazz, "mDetectLandmarks", "J");
            env->DeleteLocalRef(clazz);
        }

        DetectorPtr_DLIB getDetectorPtrFromJava(JNIEnv *env, jobject thiz) {
            DetectorPtr_DLIB const p = (DetectorPtr_DLIB) env->GetLongField(thiz, mNativeContext);
            return p;
        }

        DetectorPtr2 getDetectorPtr2FromJava(JNIEnv *env, jobject thiz) {
            DetectorPtr2 const p = (DetectorPtr2) env->GetLongField(thiz, mNativeContext2);
            return p;
        }

        void setDetectorPtrToJava(JNIEnv *env, jobject thiz, jlong ptr) {
            env->SetLongField(thiz, mNativeContext, ptr);
        }

        void setDetectorPtr2ToJava(JNIEnv *env, jobject thiz, jlong ptr) {
            env->SetLongField(thiz, mNativeContext2, ptr);
        }

        void setTimeSpent(JNIEnv *env, jobject thiz, jlong ptr) {
            env->SetLongField(thiz, mTimeSpent, ptr);
        }

        bool getDetectLandmarksFlag(JNIEnv *env, jobject thiz) {
            return (bool) env->GetLongField(thiz, mDetectLandmarks);
        }

        jfieldID mTimeSpent;
        jfieldID mNativeContext;
        jfieldID mNativeContext2;
        jfieldID mDetectLandmarks;
    };

// Protect getting/setting and creating/deleting pointer between java/native
    //std::mutex gLock;

    std::shared_ptr<JNI_FaceDetector> getJNI_FaceDetector(JNIEnv *env) {
        static std::once_flag sOnceInitflag;
        static std::shared_ptr<JNI_FaceDetector> sJNI_FaceDetector;
        std::call_once(sOnceInitflag, [env]() {
            sJNI_FaceDetector = std::make_shared<JNI_FaceDetector>(env);
        });
        return sJNI_FaceDetector;
    }

    DetectorPtr_DLIB const getDetectorPtrDlib(JNIEnv *env, jobject thiz) {
        std::lock_guard<std::mutex> lock(gLock);
        return getJNI_FaceDetector(env)->getDetectorPtrFromJava(env, thiz);
    }

    DetectorPtr2 const getDetectorPtr2(JNIEnv *env, jobject thiz) {
        std::lock_guard<std::mutex> lock(gLock);
        return getJNI_FaceDetector(env)->getDetectorPtr2FromJava(env, thiz);
    }
// The function to set a pointer to java and delete it if newPtr is empty
    void setDetectorPtrDLib(JNIEnv *env, jobject thiz, DetectorPtr_DLIB newPtr) {
        std::lock_guard<std::mutex> lock(gLock);
        DetectorPtr_DLIB oldPtr = getJNI_FaceDetector(env)->getDetectorPtrFromJava(env, thiz);
        if (oldPtr != JAVA_NULL) {
            delete oldPtr;
        }

        if (newPtr != JAVA_NULL) {
            DLOG(INFO) << "setMapManager set new ptr : " << newPtr;
        }

        getJNI_FaceDetector(env)->setDetectorPtrToJava(env, thiz, (jlong) newPtr);
    }

    void setDetectorPtr2(JNIEnv *env, jobject thiz, DetectorPtr2 newPtr) {
        std::lock_guard<std::mutex> lock(gLock);
        DetectorPtr2 oldPtr = getJNI_FaceDetector(env)->getDetectorPtr2FromJava(env, thiz);
        if (oldPtr != JAVA_NULL) {
            delete oldPtr;
        }

        if (newPtr != JAVA_NULL) {
            DLOG(INFO) << "setMapManager set new ptr : " << newPtr;
        }

        getJNI_FaceDetector(env)->setDetectorPtr2ToJava(env, thiz, (jlong) newPtr);
    }
}  // end unnamespace

#ifdef __cplusplus
extern "C" {
#endif

#define DLIB_FACE_JNI_METHOD(METHOD_NAME) \
  Java_com_huawei_dlib_DlibModFaceDetector_##METHOD_NAME

void JNIEXPORT
DLIB_FACE_JNI_METHOD(jniNativeClassInit)(JNIEnv *env, jclass _this) {}

jobjectArray getDetectResult(JNIEnv *env, DetectorPtr_DLIB faceDetector, const int &size, Mat &frame,
                             bool landmarksEnabled, cv::Rect& cutRect) {

    float angles[3];
    float modelView[16];
    float frustumScale[3];
    float landmarks[2 * 68];

    //LOG(INFO) << "getDetectResult";
    jobjectArray jDetRetArray = JNI_DetectedFace::createJObjectArray(env, size);
    for (int i = 0; i < size; i++) {
        jobject jDetRet = JNI_DetectedFace::createJObject(env);
        env->SetObjectArrayElement(jDetRetArray, i, jDetRet);
        dlib::rectangle rect = faceDetector->getResult()[i];
        g_pJNI_DetectedFace->setRect(env, jDetRet,
                                     (int) rect.left() + cutRect.x,
                                     (int) rect.top() + cutRect.y,
                                     (int) rect.right() + cutRect.x,
                                     (int) rect.bottom()+ cutRect.y);
        /*g_pJNI_DetectedFace->setRect(env, jDetRet, // TODO: DELETE IT LATER!!!
                                     (int) cutRect.x,
                                     (int) cutRect.y,
                                     (int) cutRect.x + cutRect.width,
                                     (int) cutRect.y + cutRect.height);*/
        std::unordered_map<int, dlib::full_object_detection> &faceShapeMap =
                faceDetector->getFaceShapeMap();
        if (landmarksEnabled && faceShapeMap.find(i) != faceShapeMap.end()) {
            dlib::full_object_detection shape = faceShapeMap[i];
            for (unsigned long j = 0; j < shape.num_parts(); j++) {
                int x = (int) shape.part(j).x() + cutRect.x;
                int y = (int) shape.part(j).y() + cutRect.y;
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
            model_points.push_back(cv::Point3d(-225.0f, 170.0f, -135.0f));
            // Right eye right corner
            model_points.push_back(cv::Point3d(225.0f, 170.0f, -135.0f));
            // Left Mouth corner
            model_points.push_back(cv::Point3d(-150.0f, -150.0f, -125.0f));
            // Right mouth corner
            model_points.push_back(cv::Point3d(150.0f, -150.0f, -125.0f));

            // Camera Approximate
            double focal_length = cutRect.width; // Approximate focal length.
            cv::Point2d center = cv::Point2d(cutRect.width / 2, cutRect.height / 2);
            cv::Mat camera_matrix = (cv::Mat_<double>(3, 3) << focal_length, 0, center.x,
                    0, focal_length, center.y, 0, 0, 1);
            // Assuming no lens distortion
            cv::Mat dist_coeffs = cv::Mat::zeros(4, 1, cv::DataType<double>::type);

            // 2D image points. If you change the image, you need to change vector
            // Y at image top is 0 (bottom is image height)
            // Numbers coming from landmark detection model
            dlib::dpoint noseTip = shape.part(30);
            dlib::dpoint chin = shape.part(8);
            dlib::dpoint leftEyeLeftCorner = shape.part(36);
            dlib::dpoint rightEyeRightCorner = shape.part(45);
            dlib::dpoint leftMouthCorner = shape.part(48);
            dlib::dpoint rightMouthCorner = shape.part(54);

            std::vector<cv::Point2d> image_points;
            // Nose tip
            image_points.push_back(cv::Point2d(noseTip.x(), noseTip.y()));
            // Chin
            image_points.push_back(cv::Point2d(chin.x(), chin.y()));
            // Left eye left corner
            image_points.push_back(cv::Point2d(leftEyeLeftCorner.x(), leftEyeLeftCorner.y()));
            // Right eye right corner
            image_points.push_back(cv::Point2d(rightEyeRightCorner.x(), rightEyeRightCorner.y()));
            // Left Mouth corner
            image_points.push_back(cv::Point2d(leftMouthCorner.x(), leftMouthCorner.y()));
            // Right mouth corner
            image_points.push_back(cv::Point2d(rightMouthCorner.x(), rightMouthCorner.y()));

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

            jfieldID landmarksFId = env->GetFieldID(jcClass, "mLandmarks", "[F");
            jobject landmarksObj = env->GetObjectField(jDetRet, landmarksFId);
            jfloatArray *landmarksArray = reinterpret_cast<jfloatArray *>(&landmarksObj);

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

            // Copy landmarks, make them relative to frame
            for (unsigned j = 0; j < 68; j++) {
                dlib::dpoint rel = shape.part(j);
                landmarks[j * 2] = (float) (rel.x() / frame.cols) - 0.5f;
                landmarks[j * 2 + 1] = (float) (rel.y() / frame.rows) - 0.5f;
            }
            env->SetFloatArrayRegion(*landmarksArray, 0, 2 * 68, landmarks);

            std::vector<cv::Point3d> nose_end_point3D;
            std::vector<cv::Point2d> nose_end_point2D;
            nose_end_point3D.push_back(cv::Point3d(0, 0, 1000.0));
            cv::projectPoints(nose_end_point3D, rotation_vector, translation_vector, camera_matrix,
                              dist_coeffs, nose_end_point2D);

            g_pJNI_DetectedFace->setNosePoint(env, jDetRet,
                                              (int) nose_end_point2D.data()->x,
                                              (int) nose_end_point2D.data()->y);

            float mouthOpen = 0.0f;
            dlib::dpoint topMouth = shape.part(62);
            dlib::dpoint bottomMouth = shape.part(66);
            if (bottomMouth.y() > topMouth.y()) {
                int mx = (int) (bottomMouth.x() - topMouth.x());
                int my = (int) (bottomMouth.y() - topMouth.y());
                float mouth = (float) sqrt(mx * mx + my * my);
                dlib::dpoint underlip = shape.part(57);
                mx = (int) (chin.x() - underlip.x());
                my = (int) (chin.y() - underlip.y());
                float lip = (float) sqrt(mx * mx + my * my);
                mouthOpen = mouth / lip;
                mouthOpen = mouthOpen > 1.0f ? 1.0f : mouthOpen;
            }
            g_pJNI_DetectedFace->setMouthOpen(env, jDetRet, mouthOpen);
        }
    }
    return jDetRetArray;
}

/* return current time in milliseconds */
static long getSystemTime(void) {
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    return (long) (1000.0 * res.tv_sec + (double) res.tv_nsec / 1e6);
}

void alignToGrid(int& x1, int& y1, int& x2, int& y2, const cv::Size& sz, int cellCount) {
    float wStep = (float) sz.width / cellCount;
    float hStep = (float) sz.height / cellCount;
    int cellX1 = (int) round((float) x1 / wStep);
    int cellY1 = (int) round((float) y1 / hStep);
    int cellX2 = (int) round((float) x2 / wStep);
    int cellY2 = (int) round((float) y2 / hStep);
    x1 = cellX1 * (int) wStep;
    y1 = cellY1 * (int) hStep;
    x2 = cellX2 * (int) wStep;
    y2 = cellY2 * (int) hStep;
}

JNIEXPORT jobjectArray JNICALL
DLIB_FACE_JNI_METHOD(jniBitmapDetect)(JNIEnv *env, jobject thiz, jobject bitmap) {
    //LOG(INFO) << "jniBitmapDetect";
    cv::Mat rgbaMat;
    cv::Mat bgrMat;
    DetectorPtr_DLIB detPtr = getDetectorPtrDlib(env, thiz);
    DetectorPtr2 detPtr2 = getDetectorPtr2(env, thiz);
    bool landmarksEnabled = getJNI_FaceDetector(env)->getDetectLandmarksFlag(env, thiz);
    detPtr->setLandmarksFlag(landmarksEnabled);
    long start = getSystemTime();
    jniutils::ConvertBitmapToRGBAMat(env, bitmap, rgbaMat, true);
    cv::cvtColor(rgbaMat, bgrMat, cv::COLOR_RGBA2BGR);

    // Find face rectangles
    vector<Rect> faces;
    cv::Mat bgrMat2;
    cv::cvtColor(rgbaMat, bgrMat2, cv::COLOR_RGB2GRAY);
    detPtr2->detect(bgrMat2, faces);
    jint size = faces.size();
    cv::Rect cutRect;
    if (size) {
        cv::Rect rect = faces.at(0);
        int x1 = rect.x;
        int y1 = rect.y;
        int x2 = rect.x + rect.width;
        int y2 = rect.y + rect.height;
        int extraW = (x2 - x1) / 2;
        int extraH = (y2 - y1) / 4;
        x1 = std::max(0, x1 - extraW);
        y1 = std::max(0, y1 - extraH);
        x1 = std::min(bgrMat.cols, x1);
        y1 = std::min(bgrMat.rows, y1);
        x2 = std::min(bgrMat.cols, x2 + extraW);
        y2 = std::min(bgrMat.rows, y2 + extraH);
        alignToGrid(x1, y1, x2, y2, bgrMat.size(), 10);
        //LOG(WARNING) << x1 << ":" << y1  << ":" << x2  << ":" << y2 ;
        //LOG(WARNING) << bgrMat.cols << ":" << bgrMat.rows;
        cv::Mat subMat = bgrMat(cutRect = cv::Rect(x1, y1, x2 - x1, y2 - y1));
        size = detPtr->det(subMat);
    }

    long end = getSystemTime();
    getJNI_FaceDetector(env)->setTimeSpent(env, thiz, end - start);

    //LOG(INFO) << "det num faces: " << size;
    return getDetectResult(env, detPtr, size, bgrMat, landmarksEnabled, cutRect);
}

jint JNIEXPORT JNICALL
DLIB_FACE_JNI_METHOD(jniInit)(JNIEnv *env, jobject thiz, jstring jLandmarkPath, jstring jCascadePath) {
    LOG(INFO) << "jniInit";
    std::string landmarkPath = jniutils::convertJStrToString(env, jLandmarkPath);
    DetectorPtr_DLIB detPtr = new DLibHOGFaceDetector(landmarkPath);
    setDetectorPtrDLib(env, thiz, detPtr);

    std::string cascadePath = jniutils::convertJStrToString(env, jCascadePath);
    cv::Ptr<cv::CascadeClassifier> cascade = makePtr<cv::CascadeClassifier>(cascadePath);
    cv::Ptr<DetectionBasedTracker::IDetector> MainDetector = makePtr<CascadeDetectorAdapter>(
            cascade);
    cascade = makePtr<cv::CascadeClassifier>(cascadePath);
    cv::Ptr<DetectionBasedTracker::IDetector> TrackingDetector = makePtr<CascadeDetectorAdapter>(
            cascade);
    DetectionBasedTracker::Parameters params;
    DetectorPtr2 detPtr2 = new LbpDetector(MainDetector, TrackingDetector, params);
    detPtr2->setMinObjectSize(Size(20, 20));
    detPtr2->setMinNeighbours(1);
    setDetectorPtr2(env, thiz, detPtr2);
    return JNI_OK;
}

jint JNIEXPORT JNICALL
DLIB_FACE_JNI_METHOD(jniDeInit)(JNIEnv *env, jobject thiz) {
    LOG(INFO) << "jniDeInit";
    setDetectorPtrDLib(env, thiz, JAVA_NULL);
    setDetectorPtr2(env, thiz, JAVA_NULL);
    return JNI_OK;
}

#ifdef __cplusplus
}
#endif
