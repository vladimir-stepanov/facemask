//!
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Input/InputSource.hpp>
#include <Grapevine/Core/Util.hpp>
#include <Grapevine/Core/Exception.hpp>
#include <Grapevine/Resource/ResourceManager.hpp>
#include <Grapevine/Text/FontManager.hpp>
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
    JNIEXPORT void JNICALL Java_com_barrelhello_BarrelHelloLib_onCreate(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_barrelhello_BarrelHelloLib_surfaceChanged(JNIEnv * env, jobject obj, jint width, jint height);
    JNIEXPORT void JNICALL Java_com_barrelhello_BarrelHelloLib_onDestroy(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_barrelhello_BarrelHelloLib_touch(JNIEnv * env, jobject obj, jfloat x, jfloat y, jfloat dx, jfloat dy, jint flags);
    JNIEXPORT void JNICALL Java_com_barrelhello_BarrelHelloLib_step(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_barrelhello_BarrelHelloLib_setAssetManager(JNIEnv * env, jobject obj, jobject assetManager);
};

// Global stuff (TODO: needs freeing)
ResourceManager* resourceManager_ = NULL;
FontManager* fontManager_ = NULL;
MainView* mainView_ = NULL;
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

JNIEXPORT void JNICALL Java_com_barrelhello_BarrelHelloLib_surfaceChanged(JNIEnv * env, jobject obj, jint width, jint height)
{
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

JNIEXPORT void JNICALL Java_com_barrelhello_BarrelHelloLib_onCreate(JNIEnv * env, jobject obj)
{
    LOGD("Java_com_barrelhello_BarrelHelloLib_onCreate");

    resourceManager_ = new ResourceManager();
    fontManager_ = new FontManager();
    mainView_ = NULL;
}

JNIEXPORT void JNICALL Java_com_barrelhello_BarrelHelloLib_onDestroy(JNIEnv * env, jobject obj)
{
    LOGD("Java_com_barrelhello_BarrelHelloLib_onDestroy");

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

    if (fontManager_ != NULL)
    {
        delete fontManager_;
        fontManager_ = NULL;
    }
}

JNIEXPORT void JNICALL Java_com_barrelhello_BarrelHelloLib_touch(JNIEnv * env, jobject obj, jfloat x, jfloat y, jfloat dx, jfloat dy, jint flags)
{
    LOGD("Java_touch (x:%6.1f, y:%6.1f)(dx:%6.1f, dy:%6.1f)(flags:%2.0d)",  x, y, dx, dy, flags);
    if (mainView_ != NULL)
    {
        mainView_->inputSource()->addTouchEvent(0, x, y, x - dx, y - dy, flags);
    }
}

JNIEXPORT void JNICALL Java_com_barrelhello_BarrelHelloLib_step(JNIEnv * env, jobject obj)
{
    if (mainView_ == NULL)
    {
        try
        {
            LOGD("Java_step, 1st step, creating MainView...");
            mainView_ = new MainView(w_, h_);
        }
        catch (Exception e)
        {
            if (glErrorShown == false)
                LOGE("Grapevine::Exception: %s", e.what());
            glErrorShown = true;
        }
        catch (...)
        {
            if (glErrorShown == false)
                LOGE("Unknown exception!");
            glErrorShown = true;
        }
    }

    try
    {
        mainView_->step();
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

JNIEXPORT void JNICALL Java_com_barrelhello_BarrelHelloLib_setAssetManager(JNIEnv * env, jobject obj, jobject javaAssetManager)
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
