LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
include $(NGOS_VARS)

LOCAL_MODULE := libgrapevine_png
LOCAL_SRC_FILES := \
png.c \
pngerror.c \
pngget.c \
pngmem.c \
pngpread.c \
pngread.c \
pngrio.c \
pngrtran.c \
pngrutil.c \
pngset.c \
pngtrans.c \
pngwio.c \
pngwrite.c \
pngwtran.c \
pngwutil.c

LOCAL_SHARED_LIBRARIES := libz
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)

include $(BUILD_SHARED_LIBRARY)
