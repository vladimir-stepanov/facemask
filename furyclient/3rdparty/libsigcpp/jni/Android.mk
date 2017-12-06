LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libsigcpp
LOCAL_C_INCLUDES := $(SOURCE)
LOCAL_CFLAGS := -DANDROID_NDK -DPLAY_MOD
LOCAL_CPP_EXTENSION := .cc

LOCAL_SRC_FILES := \
sigc++/signal.cc \
sigc++/signal_base.cc \
sigc++/connection.cc \
sigc++/trackable.cc \
sigc++/functors/slot_base.cc \
sigc++/functors/slot.cc \
sigc++/adaptors/lambda/lambda.cc

LOCAL_CC_INCLUDES += \
$(LOCAL_PATH) \
$(LOCAL_PATH)/functors \
$(LOCAL_PATH)/adaptors \
$(LOCAL_PATH)/adaptors/lamda \
LOCAL_CFLAGS += -fvisibility=hidden

LOCAL_LDLIBS := -lz


include $(BUILD_SHARED_LIBRARY)

