# LOCAL_PATH:= $(call my-dir) # call this in parent folder

include $(CLEAR_VARS)
include $(NGOS_VARS)

LOCAL_MODULE := libgrapevine-foundation
LOCAL_SRC_FILES:=\
	src/Arrange/Grid.cpp \
	src/Arrange/Item.cpp \
	src/Arrange/Move.cpp \
	src/Core/Condition.cpp \
	src/Core/Log.cpp \
	src/Core/Mutex.cpp \
	src/Core/RecursiveMutex.cpp \
	src/Core/Referenced.cpp \
	src/Core/SharedObject.cpp \
	src/Core/Thread.cpp \
	src/Core/Timer.cpp \
	src/GPU/GL.cpp \
	src/Image/ImageAsset.cpp \
	src/Image/ImagePng.cpp \
	src/Image/ImageJpeg.cpp \
	src/Image/ImageKtx.cpp \
	src/Image/ImageResource.cpp \
	src/Image/GeneralImageResource.cpp \
	src/Material/CubemapAsset.cpp \
	src/Material/CubemapResource-GLES2.cpp \
	src/Material/MaterialAsset.cpp \
	src/Material/MaterialResource.cpp \
	src/Material/TextureAsset.cpp \
	src/Material/TextureResource-GLES2.cpp \
	src/Material/FramebufferAsset.cpp \
	src/Math/Box.cpp \
	src/Math/Cylinder.cpp \
	src/Math/Matrix.cpp \
	src/Math/Plane.cpp \
	src/Math/Ray.cpp \
	src/Math/Sphere.cpp \
	src/Math/Vector.cpp \
	src/Model/Barrel.cpp \
	src/Model/DynamicMesh.cpp \
	src/Model/DynamicMeshPrivate-GLES2.cpp \
	src/Model/MeshAsset.cpp \
	src/Model/MeshResource-GLES2.cpp \
	src/Model/ModelAsset.cpp \
	src/Model/ModelData.cpp \
	src/Model/ModelFactory.cpp \
	src/Model/ModelResource.cpp \
	src/Resource/File.cpp \
	src/Resource/Resource.cpp \
	src/Resource/ResourceManager.cpp \
	src/Resource/ResourceManagerPrivate.cpp \
	src/Resource/MemoryFile.cpp \
	src/Shader/FragmentSource.cpp \
	src/Shader/Shader-GLES2.cpp \
	src/Shader/ShaderManager.cpp \
	src/Shader/ShaderManagerPrivate.cpp \
	src/Shader/ShaderProgram-GLES2.cpp \
	src/Shader/ShaderProgramSymbol-GLES2.cpp \
	src/Shader/ShaderSource-GLES2.cpp \
	src/Shader/VertexSource.cpp \
	src/StateMachine/State.cpp \
	src/StateMachine/StateMachine.cpp \
	src/StateMachine/StateTransition.cpp \
	src/Benchmark/FpsCounter.cpp \
	src/Benchmark/DualScrollTypeHwcSwapsFpsLogger.cpp

LOCAL_SRC_FILES += src/Animation/AnimationManager.cpp
LOCAL_SRC_FILES += src/Animation/ControlPoint.cpp
LOCAL_SRC_FILES += src/Animation/ControlPointPrivate.cpp
LOCAL_SRC_FILES += src/Animation/Animation.cpp
LOCAL_SRC_FILES += src/Animation/AnimationPrivate.cpp
LOCAL_SRC_FILES += src/Animation/LinearPath.cpp
LOCAL_SRC_FILES += src/Animation/LinearPathPrivate.cpp
LOCAL_SRC_FILES += src/Animation/BezierQuadraticPath.cpp
LOCAL_SRC_FILES += src/Animation/BezierQuadraticPathPrivate.cpp
LOCAL_SRC_FILES += src/Animation/BezierComplexPath.cpp
LOCAL_SRC_FILES += src/Animation/BezierComplexPathPrivate.cpp
LOCAL_SRC_FILES += src/Animation/Path.cpp
LOCAL_SRC_FILES += src/Animation/Easing.cpp
LOCAL_SRC_FILES += src/Input/InputSource-NGOS.cpp
LOCAL_SRC_FILES += src/Input/InputSourcePrivate.cpp
LOCAL_SRC_FILES += src/Input/InputSourcePrivateNGOS.cpp
LOCAL_SRC_FILES += src/Input/SyncedSlideEvent.cpp
LOCAL_SRC_FILES += src/Canvas/Canvas-EGL.cpp
LOCAL_SRC_FILES += src/Canvas/CanvasPrivate.cpp

LOCAL_EXPORT_C_INCLUDE_DIRS := \
    $(LOCAL_PATH)/src \
    $(NGOS_BASE)/api/include \
    $(TOP)/external/jpeg

LOCAL_C_INCLUDES += $(LOCAL_EXPORT_C_INCLUDE_DIRS)
LOCAL_SHARED_LIBRARIES := liblog libsigcpp libngdisplay libgrapevine_png libzip libGLESv2 libEGL libGLESv1_CM libutils libjpeg

ifeq ($(TARGET_PRODUCT), full_hammerhead)
LOCAL_SRC_FILES += src/Input/InputSourcePrivateNGOS-Hammerhead.cpp
else ifeq ($(TARGET_PRODUCT), GRACE)
LOCAL_SRC_FILES += src/Input/InputSourcePrivateNGOS-Grace.cpp
LOCAL_CFLAGS += -DPOWER_DEV="\"/dev/input/event1\""
LOCAL_CFLAGS += -DVOLUME_DEV="\"/dev/input/event0\""
LOCAL_CFLAGS += -DTOUCH_DEV="\"/dev/input/event4\""
else ifeq ($(TARGET_PRODUCT), NEXT)
LOCAL_SRC_FILES += src/Input/InputSourcePrivateNGOS-Grace.cpp
LOCAL_CFLAGS += -DPOWER_NAME="\"hisi_on\""
LOCAL_CFLAGS += -DVOLUME_DEV="\"/dev/input/event1\""
LOCAL_CFLAGS += -DTOUCH_NAME="\"huawei,touchscreen\""
else ifeq
LOCAL_SRC_FILES += src/Input/InputSourcePrivateNGOS-Dummy.cpp
endif

include $(BUILD_SHARED_LIBRARY)
