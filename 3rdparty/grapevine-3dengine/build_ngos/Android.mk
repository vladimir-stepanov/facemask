# LOCAL_PATH:= $(call my-dir) # call this in parent folder

include $(CLEAR_VARS)

LOCAL_MODULE := libgrapevine-3dengine

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES:=\
    src/Scene/AbstractComponentPrivate.cpp \
    src/Scene/AbstractListVisitor.cpp \
    src/Scene/AbstractTreeVisitor.cpp \
    src/Scene/AbstractVisitor.cpp \
    src/Scene/Component.cpp \
    src/Scene/Graph.cpp \
    src/Scene/GraphPrivate.cpp \
    src/Scene/Node.cpp \
    src/Scene/NodeList.cpp \
    src/Scene/NodeListPrivate.cpp \
    src/Scene/NodePrivate.cpp \
    src/SceneExtensions/AbstractRenderableComponent.cpp \
    src/SceneExtensions/AbstractRenderableComponentPrivate.cpp \
    src/SceneExtensions/AnimationComponent.cpp \
    src/SceneExtensions/AnimationComponentPrivate.cpp \
    src/SceneExtensions/AnimationProgressTriggerComponent.cpp \
    src/SceneExtensions/AnimationProgressTriggerComponentPrivate.cpp \
    src/SceneExtensions/AnimatorVisitor.cpp \
    src/SceneExtensions/BoundingVolumeComponent.cpp \
    src/SceneExtensions/BoundingVolumeComponentPrivate.cpp \
    src/SceneExtensions/CameraComponent.cpp \
    src/SceneExtensions/CameraComponentPrivate.cpp \
    src/SceneExtensions/CullerBoxVisitor.cpp \
    src/SceneExtensions/CullerVisitor.cpp \
    src/SceneExtensions/InfoComponent.cpp \
    src/SceneExtensions/InfoComponentPrivate.cpp \
    src/SceneExtensions/InputComponent.cpp \
    src/SceneExtensions/InputComponentPrivate.cpp \
    src/SceneExtensions/InputVisitor.cpp \
    src/SceneExtensions/ListAnimatorVisitor.cpp \
    src/SceneExtensions/ModelComponent.cpp \
    src/SceneExtensions/ModelComponentPrivate.cpp \
    src/SceneExtensions/Renderer-GLES2.cpp \
    src/SceneExtensions/SimpleScene.cpp \
    src/SceneExtensions/TransformVisitor.cpp \
    src/SceneExtensions/TransformationComponent.cpp \
    src/SceneExtensions/TransformationComponentPrivate.cpp

LOCAL_C_INCLUDES := \
        $(LOCAL_PATH)/include

LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_C_INCLUDES)

LOCAL_C_INCLUDES += $(LOCAL_PATH)/src

LOCAL_SHARED_LIBRARIES := liblog libsigcpp libgrapevine libgrapevine_png libiconv libGLESv2 libEGL libGLESv1_CM

LOCAL_CFLAGS            := -fexceptions -frtti
LOCAL_CFLAGS            += -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -D__NGOS__
LOCAL_NDK_STL_VARIANT   := gnustl_static
LOCAL_SDK_VERSION       := 18
LOCAL_LDFLAGS           := -Wl,--no-fatal-warnings

include $(BUILD_SHARED_LIBRARY)
