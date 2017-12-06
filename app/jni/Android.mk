LOCAL_PATH := $(call my-dir)
SUB_MK_FILES := $(call all-subdir-makefiles)

## Build dlib to static library
include $(CLEAR_VARS)

TOP_LEVEL_PATH := $(abspath $(LOCAL_PATH)/../..)
## $(info TOP Level Path: $(TOP_LEVEL_PATH))

OPENCV_PATH = $(TOP_LEVEL_PATH)/3rdparty/opencv/jni
OPENCV_INCLUDE_DIR = $(OPENCV_PATH)/include

MINIGLOG_LIB_TYPE := STATIC
MINI_GLOG_PATH = $(TOP_LEVEL_PATH)/3rdparty/miniglog
include $(MINI_GLOG_PATH)/Android.mk

include $(SUB_MK_FILES)
