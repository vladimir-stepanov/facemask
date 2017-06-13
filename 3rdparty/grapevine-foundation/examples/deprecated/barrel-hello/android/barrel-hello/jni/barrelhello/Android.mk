LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../../../../ \
	$(LOCAL_PATH)/../../../../../../../grapevine/include \
	$(LOCAL_PATH)/../../../../../../../libpng-android/jni \
	$(LOCAL_PATH)/../../../../../../../libsigcpp-android/jni \
    $(LOCAL_PATH)/../../../../../../../libiconv-android/libiconv-1.14/include \
	$(LOCAL_PATH)/../../../../../../../src/Canvas \
	$(LOCAL_PATH)/../../../../../../../src/Image \
	$(LOCAL_PATH)/../../../../../../../grapevine/src/  \
	$(LOCAL_PATH)/../../../../../../../libzip-android/jni

LOCAL_SHARED_LIBRARIES      := libgrapevine_png libsigcpp libgrapevine libiconv
LOCAL_MODULE                := libbarrelhello
LOCAL_CFLAGS                := -Werror
LOCAL_LDLIBS                := -llog -lGLESv2 -lz -landroid
LOCAL_SRC_FILES             := ../jni.cpp \
../../../../MainView.cpp

include $(BUILD_SHARED_LIBRARY)
