LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := grapevine-3dengine

SOURCE := ../../../../src

LOCAL_C_INCLUDES := \
$(LOCAL_PATH)/../../../../../libzip-android/jni \
$(LOCAL_PATH)/../../../../../libpng-android/jni \
$(LOCAL_PATH)/../../../../../libsigcpp-android/jni \
$(LOCAL_PATH)/../../../../../libiconv-android/libiconv-1.14/include \
$(LOCAL_PATH)/../../../../include \
$(LOCAL_PATH)/../../../../../grapevine/include \

# To print out local path:
# $(warning $(LOCAL_PATH))

# Application source files:
LOCAL_SRC_FILES := \
\
$(SOURCE)/Scene/AbstractVisitor.cpp \
$(SOURCE)/Scene/AbstractTreeVisitor.cpp \
$(SOURCE)/Scene/AbstractListVisitor.cpp \
$(SOURCE)/Scene/AbstractComponentPrivate.cpp \
$(SOURCE)/Scene/Component.cpp \
$(SOURCE)/Scene/Graph.cpp \
$(SOURCE)/Scene/GraphPrivate.cpp \
$(SOURCE)/Scene/Node.cpp \
$(SOURCE)/Scene/NodePrivate.cpp \
$(SOURCE)/Scene/NodeList.cpp \
$(SOURCE)/Scene/NodeListPrivate.cpp \
\
$(SOURCE)/SceneExtensions/AnimationComponent.cpp \
$(SOURCE)/SceneExtensions/AnimationComponentPrivate.cpp \
$(SOURCE)/SceneExtensions/AnimationProgressTriggerComponent.cpp \
$(SOURCE)/SceneExtensions/AnimationProgressTriggerComponentPrivate.cpp \
$(SOURCE)/SceneExtensions/AnimatorVisitor.cpp \
$(SOURCE)/SceneExtensions/ListAnimatorVisitor.cpp \
$(SOURCE)/SceneExtensions/CameraComponent.cpp \
$(SOURCE)/SceneExtensions/CameraComponentPrivate.cpp \
$(SOURCE)/SceneExtensions/CullerBoxVisitor.cpp \
$(SOURCE)/SceneExtensions/CullerVisitor.cpp \
$(SOURCE)/SceneExtensions/InfoComponent.cpp \
$(SOURCE)/SceneExtensions/InfoComponentPrivate.cpp \
$(SOURCE)/SceneExtensions/InputComponent.cpp \
$(SOURCE)/SceneExtensions/InputComponentPrivate.cpp \
$(SOURCE)/SceneExtensions/InputVisitor.cpp \
$(SOURCE)/SceneExtensions/AbstractRenderableComponent.cpp \
$(SOURCE)/SceneExtensions/AbstractRenderableComponentPrivate.cpp \
$(SOURCE)/SceneExtensions/ModelComponent.cpp \
$(SOURCE)/SceneExtensions/ModelComponentPrivate.cpp \
$(SOURCE)/SceneExtensions/Renderer-GLES2.cpp \
$(SOURCE)/SceneExtensions/SimpleScene.cpp \
$(SOURCE)/SceneExtensions/TextComponent.cpp \
$(SOURCE)/SceneExtensions/TextComponentPrivate.cpp \
$(SOURCE)/SceneExtensions/TransformVisitor.cpp \
$(SOURCE)/SceneExtensions/TransformationComponent.cpp \
$(SOURCE)/SceneExtensions/TransformationComponentPrivate.cpp \
$(SOURCE)/SceneExtensions/BoundingVolumeComponent.cpp \
$(SOURCE)/SceneExtensions/BoundingVolumeComponentPrivate.cpp \


# Dependencies
LOCAL_STATIC_LIBRARIES := libzip
LOCAL_SHARED_LIBRARIES := libgrapevine libgrapevine_png libsigcpp libiconv
LOCAL_STATIC_LIBRARY   := libgnustl
LOCAL_LDLIBS           := -llog -lz -lm -landroid -lGLESv2
LOCAL_CFLAGS           := -Wall -g

include $(BUILD_SHARED_LIBRARY)

