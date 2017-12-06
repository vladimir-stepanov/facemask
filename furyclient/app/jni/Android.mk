LOCAL_PATH := $(call my-dir)
SUB_MK_FILES := $(call all-subdir-makefiles)

## Build dlib to static library
include $(CLEAR_VARS)
LOCAL_MODULE := dlib
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../3rdparty/dlib

LOCAL_SRC_FILES += \
                ../$(LOCAL_C_INCLUDES)/dlib/threads/threads_kernel_shared.cpp \
                ../$(LOCAL_C_INCLUDES)/dlib/entropy_decoder/entropy_decoder_kernel_2.cpp \
                ../$(LOCAL_C_INCLUDES)/dlib/base64/base64_kernel_1.cpp \
                ../$(LOCAL_C_INCLUDES)/dlib/threads/threads_kernel_1.cpp \
                ../$(LOCAL_C_INCLUDES)/dlib/threads/threads_kernel_2.cpp

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)
include $(BUILD_STATIC_LIBRARY)

TOP_LEVEL_PATH := $(abspath $(LOCAL_PATH)/../..)
## $(info TOP Level Path: $(TOP_LEVEL_PATH))

OPENCV_PATH = $(TOP_LEVEL_PATH)/3rdparty/opencv/jni
OPENCV_INCLUDE_DIR = $(OPENCV_PATH)/include

MINIGLOG_LIB_TYPE := STATIC
MINI_GLOG_PATH = $(TOP_LEVEL_PATH)/3rdparty/miniglog
include $(MINI_GLOG_PATH)/Android.mk

include $(SUB_MK_FILES)
