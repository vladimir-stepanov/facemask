//!
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Input/InputSource.hpp>
#include <Grapevine/Core/Util.hpp>
#include <Grapevine/Core/Exception.hpp>
#include <Grapevine/Resource/ResourceManager.hpp>
#include <Grapevine/Text/FontManager.hpp>
#include "MainView.hpp"
#include <string>
#include <jni.h>

using namespace Grapevine;

// Cached references for later callbacks
JavaVM * g_vm = NULL;
jobject g_obj = NULL;
jmethodID g_mid = NULL;
bool glErrorShown = false;

extern "C" {
    JNIEXPORT void JNICALL Java_com_example_unlock_UnlockLib_init(JNIEnv * env, jobject obj, jint width, jint height);
    JNIEXPORT void JNICALL Java_com_example_unlock_UnlockLib_touch(JNIEnv * env, jobject obj, jfloat x, jfloat y, jfloat dx, jfloat dy, jint flags);
    JNIEXPORT void JNICALL Java_com_example_unlock_UnlockLib_step(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_example_unlock_UnlockLib_setAssetManager(JNIEnv * env, jobject obj, jobject assetManager);
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

    resourceManager_ = new ResourceManager();
    fontManager_ = new FontManager();

    return JNI_VERSION_1_6;
}

void JNI_OnUnLoad(JavaVM* vm, void* reserved)
{
    LOGD("JNI_OnUnLoad");

    delete resourceManager_;
    delete fontManager_;
    delete mainView_;
}

JNIEXPORT void JNICALL Java_com_example_unlock_UnlockLib_init(JNIEnv * env, jobject obj, jint width, jint height)
{
    mainView_ = NULL;
    w_ = width;
    h_ = height;
}

JNIEXPORT void JNICALL Java_com_example_unlock_UnlockLib_touch(JNIEnv * env, jobject obj, jfloat x, jfloat y, jfloat dx, jfloat dy, jint flags)
{
    LOGD("Java_touch (x:%6.1f, y:%6.1f)(dx:%6.1f, dy:%6.1f)(flags:%2.0d)",  x, y, dx, dy, flags);
    if (mainView_ != NULL)
    {
        mainView_->inputSource()->addTouchEvent(0, x, y, x - dx, y - dy, flags);
    }
}

JNIEXPORT void JNICALL Java_com_example_unlock_UnlockLib_step(JNIEnv * env, jobject obj)
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

JNIEXPORT void JNICALL Java_com_example_unlock_UnlockLib_setAssetManager(JNIEnv * env, jobject obj, jobject javaAssetManager)
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
