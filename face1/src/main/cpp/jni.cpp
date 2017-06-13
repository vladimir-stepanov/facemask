//!
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Input/InputEvent.hpp>
#include <Grapevine/Resource/ResourceManager.hpp>
#include <Grapevine/Core/Util.hpp>
#include <Grapevine/Core/Exception.hpp>
#include <Grapevine/Core/String.hpp>
#include <string>
#include <jni.h>
#include "MainView.hpp"

using namespace Grapevine;

// Cached references for later callbacks
JavaVM * g_vm = NULL;
jobject g_obj = NULL;
jmethodID g_mid = NULL;
bool glErrorShown = false;

extern "C" {
    JNIEXPORT void JNICALL Java_grapevine_face1_MainLib_onCreate(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_grapevine_face1_MainLib_surfaceChanged(JNIEnv * env, jobject obj, jint width, jint height);
    JNIEXPORT void JNICALL Java_grapevine_face1_MainLib_onDestroy(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_grapevine_face1_MainLib_touch(JNIEnv * env, jobject obj, jfloat x, jfloat y, jfloat dx, jfloat dy, jint flags);
    JNIEXPORT void JNICALL Java_grapevine_face1_MainLib_step(JNIEnv * env, jobject obj, jboolean haveFace, jboolean newPose, jfloat px, jfloat py, jfloat pz, jfloatArray modelView, jfloatArray frustumScale, jfloatArray landmarks);
    JNIEXPORT void JNICALL Java_grapevine_face1_MainLib_setAssetManager(JNIEnv * env, jobject obj, jobject assetManager);
};

// Globals (TODO: needs freeing)
static ResourceManager* resourceManager_ = NULL;
static MainView* mainView_ = NULL;
unsigned int w_ = 0;
unsigned int h_ = 0;

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    LOGD("JNI_OnLoad");
    g_vm = vm;
    return JNI_VERSION_1_6;
}

void JNI_OnUnLoad(JavaVM* vm, void* reserved)
{
    LOGD("JNI_OnUnLoad");
}

JNIEXPORT void JNICALL Java_grapevine_face1_MainLib_surfaceChanged(JNIEnv * env, jobject obj, jint width, jint height)
{
    LOGD("Java_grapevine_face1_MainLib_surfaceChanged: %d x %d", width, height);

    if (mainView_ == NULL)
    {
        try
        {
            w_ = width;
            h_ = height;
            LOGD("Java_step, 1st step, creating MainView...");
            mainView_ = new MainView(w_, h_);
        }
        catch (Exception e)
        {
            LOGE("Grapevine::Exception: %s", e.what());
        }
        catch (...)
        {
            LOGE("Unknown exception!");
        }
    }
    if (mainView_ != NULL)
    {
        mainView_->surfaceChanged(w_, h_);
    }
}

JNIEXPORT void JNICALL Java_grapevine_face1_MainLib_onCreate(JNIEnv * env, jobject obj)
{
    LOGD("Java_grapevine_face1_MainLib_onCreate");

    resourceManager_ = new ResourceManager();
    mainView_ = NULL;
}

JNIEXPORT void JNICALL Java_grapevine_face1_MainLib_onDestroy(JNIEnv * env, jobject obj)
{
    LOGD("Java_grapevine_face1_MainLib_onDestroy");

    if (mainView_ != NULL)
    {
        delete mainView_;
        mainView_ = NULL;
    }

    if (resourceManager_ != NULL)
    {
        delete resourceManager_;
        resourceManager_ = NULL;
    }
}

JNIEXPORT void JNICALL Java_grapevine_face1_MainLib_touch(JNIEnv * env, jobject obj, jfloat x, jfloat y, jfloat dx, jfloat dy, jint flags)
{
    LOGD("Java_grapevine_face1_MainLib_touch");

    if (mainView_ != NULL)
    {
        Input::TouchEvent *event = new Input::TouchEvent();
        /*
        event->type = Input::InputEventType::Touch;
        event->id = 0;
        event->x = x; //coords[0];
        event->y = y; //coords[1];
        // event->t = ts[0];
        event->lastX = x - dx; //coords[2];
        event->lastY = y - dy; // coords[3];
        // event->lastT = ts[1];
        event->flags = flags;
        */
        //event->handle= reinterpret_cast<void *>(env->NewGlobalRef(motionEvent));

        //env->ReleaseFloatArrayElements(coordinates, coords, 0);
        //env->ReleaseLongArrayElements(timeStamps, ts, 0);

        //mainView_->inputSource()->addEvent((InputEvent*)event);

        //mainView_->inputSource()->addTouchEvent(0, x, y, x - dx, y - dy, flags);
    }
}

JNIEXPORT void JNICALL Java_grapevine_face1_MainLib_step(JNIEnv * env, jobject obj, jboolean haveFace, jboolean newPose, jfloat px, jfloat py, jfloat pz, jfloatArray matrix, jfloatArray frustumScale, jfloatArray landmarks)
{
    //LOGD("Java_grapevine_face1_MainLib_step");
    try
    {
        if(matrix && frustumScale && landmarks)
        {
            jfloat *matrixNative = env->GetFloatArrayElements(matrix, 0);
            jfloat *matrixNative2 = env->GetFloatArrayElements(frustumScale, 0);
            jfloat *matrixNative3 = env->GetFloatArrayElements(landmarks, 0);
            //jfloat* matrixNative = NULL;
            mainView_->step(haveFace, newPose, px, py, pz, matrixNative, matrixNative2,
                            matrixNative3);

            env->ReleaseFloatArrayElements(matrix, matrixNative, 0);
            env->ReleaseFloatArrayElements(frustumScale, matrixNative2, 0);
            env->ReleaseFloatArrayElements(frustumScale, matrixNative3, 0);
        }
    }
    catch (Exception e)
    {
        if (glErrorShown == false)
            LOGE("Exception: %s", e.what());
        glErrorShown = true;
    }
    catch (...)
    {
        if (glErrorShown == false)
            LOGE("Exception: unknown exception!");
        glErrorShown = true;
    }
}

JNIEXPORT void JNICALL Java_grapevine_face1_MainLib_setAssetManager(JNIEnv * env, jobject obj, jobject javaAssetManager)
{
    LOGD("Java_setAssetManager");

    // Set asset manager
    AAssetManager* assetManager = AAssetManager_fromJava(env, javaAssetManager);
    if (!assetManager)
    {
        LOGE("No asset manager");
        return;
    }
    ResourceManager::setAssetManager(assetManager);
}
