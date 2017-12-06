#ifndef JNI_PRIMITIVES_H
#define JNI_PRIMITIVES_H

#include <jni.h>
#include <glog/logging.h>

// Java Integer/Float
#define CLASSNAME_INTEGER "java/lang/Integer"
#define CONSTSIG_INTEGER "(I)V"
#define CLASSNAME_FLOAT "java/lang/Float"
#define CONSTSIG_FLOAT "(F)V"
#define CLASSNAME_POINTF "android/graphics/PointF"
#define CONSTSIG_POINTF "()V"

#define CLASSNAME_DETECTED_FACE "com/huawei/dlib/DlibDetectedFace"
#define CONSTSIG_DETECTED_FACE "()V"

#define CLASSNAME_FACE_DETECTOR "com/huawei/dlib/DlibFaceDetector"

class JavaPeer {
public:
    JavaPeer(JNIEnv *env, const char *className, const char *constSig) {
        cls =
                reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass(className)));
        if (env->ExceptionCheck())
            DLOG(INFO) << "Failed to find class " << className;

        constructor = env->GetMethodID(cls, "<init>", constSig);
        if (env->ExceptionCheck())
            DLOG(INFO) << "Failed to find method " << constSig;

        env->GetJavaVM(&vm);
    }

    ~JavaPeer() {
        if (!vm)
            return;

        JNIEnv *env;
        vm->GetEnv((void **) &env, JNI_VERSION_1_6);
        env->DeleteGlobalRef(cls);
    }

    jobjectArray ConstructArray(JNIEnv *env, int size) const {
        return env->NewObjectArray(size, cls, nullptr);
    }

    jobject Construct(JNIEnv *pEnv, ...) const {
        va_list args;
        va_start(args, pEnv);
        jobject obj = pEnv->NewObjectV(cls, constructor, args);
        va_end(args);

        return obj;
    }

    JavaVM *vm;
    jclass cls;
    jmethodID constructor;
};

// Java Integer/Float
class JNI_Integer : public JavaPeer {
public:
    JNI_Integer(JNIEnv *pEnv)
            : JavaPeer(pEnv, CLASSNAME_INTEGER, CONSTSIG_INTEGER) {}
};

class JNI_Float : public JavaPeer {
public:
    JNI_Float(JNIEnv *pEnv) : JavaPeer(pEnv, CLASSNAME_FLOAT, CONSTSIG_FLOAT) {}
};

struct PointF {
    float x;
    float y;
};

class JNI_PointF : public JavaPeer {
public:
    JNI_PointF(JNIEnv *pEnv) : JavaPeer(pEnv, CLASSNAME_POINTF, CONSTSIG_POINTF) {
        m_id_x = pEnv->GetFieldID(cls, "x", "F");
        m_id_y = pEnv->GetFieldID(cls, "y", "F");
    }

    void Set(JNIEnv *pEnv, const PointF &pointF, jobject objPointF) const {
        pEnv->SetFloatField(objPointF, m_id_x, pointF.x);
        pEnv->SetFloatField(objPointF, m_id_y, pointF.y);
    }

    PointF Get(JNIEnv *pEnv, jobject objPointF) const {
        PointF pointF;
        pointF.x = pEnv->GetFloatField(objPointF, m_id_x);
        pointF.y = pEnv->GetFloatField(objPointF, m_id_y);
        return pointF;
    }

private:
    jfieldID m_id_x;
    jfieldID m_id_y;
};

class JNI_DetectedFace {
public:
    JNI_DetectedFace(JNIEnv *env) {
        jclass detRetClass = env->FindClass(CLASSNAME_DETECTED_FACE);
        CHECK_NOTNULL(detRetClass);
        jID_left = env->GetFieldID(detRetClass, "mLeft", "I");
        jID_top = env->GetFieldID(detRetClass, "mTop", "I");
        jID_right = env->GetFieldID(detRetClass, "mRight", "I");
        jID_bottom = env->GetFieldID(detRetClass, "mBottom", "I");
        jID_nose3Dx = env->GetFieldID(detRetClass, "mNoseEndX", "I");
        jID_nose3Dy = env->GetFieldID(detRetClass, "mNoseEndY", "I");
        jID_mouthOpen = env->GetFieldID(detRetClass, "mMouthOpen", "F");
        jMethodID_addLandmark = env->GetMethodID(detRetClass, "addLandmark", "(II)Z");
    }

    void setRect(JNIEnv *env, jobject &jDetRet, const int &left, const int &top,
                 const int &right, const int &bottom) {
        env->SetIntField(jDetRet, jID_left, left);
        env->SetIntField(jDetRet, jID_top, top);
        env->SetIntField(jDetRet, jID_right, right);
        env->SetIntField(jDetRet, jID_bottom, bottom);
    }

    void setNosePoint(JNIEnv *env, jobject &jDetRet, const int &x, const int &y) {
        env->SetIntField(jDetRet, jID_nose3Dx, x);
        env->SetIntField(jDetRet, jID_nose3Dy, y);
    }

    void setMouthOpen(JNIEnv *env, jobject &jDetRet, const float &mouthOpen) {
        env->SetFloatField(jDetRet, jID_mouthOpen, mouthOpen);
    }

    void addLandmark(JNIEnv *env, jobject &jDetRet, const int &x, const int &y) {
        env->CallBooleanMethod(jDetRet, jMethodID_addLandmark, x, y);
    }

    static jobject createJObject(JNIEnv *env) {
        jclass detRetClass = env->FindClass(CLASSNAME_DETECTED_FACE);
        jmethodID mid =
                env->GetMethodID(detRetClass, "<init>", CONSTSIG_DETECTED_FACE);
        return env->NewObject(detRetClass, mid);
    }

    static jobjectArray createJObjectArray(JNIEnv *env, const int &size) {
        jclass detRetClass = env->FindClass(CLASSNAME_DETECTED_FACE);
        return (jobjectArray) env->NewObjectArray(size, detRetClass, NULL);
    }

private:
    jfieldID jID_left;
    jfieldID jID_top;
    jfieldID jID_right;
    jfieldID jID_bottom;
    jfieldID jID_nose3Dx;
    jfieldID jID_nose3Dy;
    jfieldID jID_mouthOpen;
    jmethodID jMethodID_addLandmark;
};

#endif  // JNI_PRIMITIVES_H
