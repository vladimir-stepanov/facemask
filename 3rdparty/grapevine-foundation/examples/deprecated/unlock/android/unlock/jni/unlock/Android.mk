LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../../../../src \
	$(LOCAL_PATH)/../../../../../../include \
	$(LOCAL_PATH)/../../../../../../../libpng-android/jni \
	$(LOCAL_PATH)/../../../../../../../libsigcpp-android/jni \
    $(LOCAL_PATH)/../../../../../../../libiconv-android/libiconv-1.14/include \
	$(LOCAL_PATH)/../../../../../../src/Canvas \
	$(LOCAL_PATH)/../../../../../../src/Image \
	$(LOCAL_PATH)/../../../../../../grapevine/src/  \
	$(LOCAL_PATH)/../../../../../../../libzip-android/jni \
	$(LOCAL_PATH)/../../../../../../../bullet-android/bullet-2.81-rev2613/src

LOCAL_SHARED_LIBRARIES      := libgrapevine_png libsigcpp libgrapevine libiconv libbullet
LOCAL_MODULE                := libunlock
LOCAL_CFLAGS                := -Werror
LOCAL_LDLIBS                := -llog -lGLESv2 -lz -landroid
LOCAL_SRC_FILES             := ../jni.cpp \
 ../../../../src/MainView.cpp

include $(BUILD_SHARED_LIBRARY)
