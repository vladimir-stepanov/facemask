LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := grapevine

SOURCE := ../../../../src

LOCAL_C_INCLUDES := \
$(LOCAL_PATH)/../../../../../libzip-android/jni \
$(LOCAL_PATH)/../../../../../libpng-android/jni \
$(LOCAL_PATH)/../../../../../libsigcpp-android/jni \
$(LOCAL_PATH)/../../../../../libiconv-android/libiconv-1.14/include \
$(LOCAL_PATH)/../../../../include \
$(LOCAL_PATH)/../../../../src/Canvas \
$(LOCAL_PATH)/../../../../src/Image

# To print out local path:
# $(warning $(LOCAL_PATH))

# Application source files:
LOCAL_SRC_FILES := \
\
$(SOURCE)/Core/Condition.cpp \
$(SOURCE)/Core/Log.cpp \
$(SOURCE)/Core/Mutex.cpp \
$(SOURCE)/Core/RecursiveMutex.cpp \
$(SOURCE)/Core/Timer.cpp \
$(SOURCE)/Core/Referenced.cpp \
$(SOURCE)/Core/Thread.cpp \
\
$(SOURCE)/Math/Vector.cpp \
$(SOURCE)/Math/Plane.cpp \
$(SOURCE)/Math/Ray.cpp \
$(SOURCE)/Math/Matrix.cpp \
$(SOURCE)/Math/Sphere.cpp \
$(SOURCE)/Math/Box.cpp \
$(SOURCE)/Math/Cylinder.cpp \
\
$(SOURCE)/Resource/File.cpp \
$(SOURCE)/Resource/Resource.cpp \
$(SOURCE)/Resource/ResourceManager.cpp \
$(SOURCE)/Resource/ResourceManagerPrivate.cpp \
\
$(SOURCE)/Benchmark/FpsCounter.cpp \
\
$(SOURCE)/GPU/GL.cpp \
\
$(SOURCE)/Canvas/CanvasPrivate.cpp \
$(SOURCE)/Canvas/Canvas-Android.cpp \
\
$(SOURCE)/Input/InputSourcePrivate.cpp \
$(SOURCE)/Input/InputSource-Android.cpp \
\
$(SOURCE)/Image/ImageResource.cpp \
$(SOURCE)/Image/ImageAsset.cpp \
$(SOURCE)/Image/ImagePng.cpp \
\
$(SOURCE)/Material/TextureResource-GLES2.cpp \
$(SOURCE)/Material/TextureAsset.cpp \
$(SOURCE)/Material/MaterialAsset.cpp \
$(SOURCE)/Material/MaterialResource.cpp \
$(SOURCE)/Material/CubemapAsset.cpp \
$(SOURCE)/Material/CubemapResource-GLES2.cpp \
\
$(SOURCE)/Model/Barrel.cpp \
$(SOURCE)/Model/ModelAsset.cpp \
$(SOURCE)/Model/ModelResource.cpp \
$(SOURCE)/Model/MeshAsset.cpp \
$(SOURCE)/Model/MeshResource-GLES2.cpp \
$(SOURCE)/Model/ModelFactory.cpp \
$(SOURCE)/Model/ModelData.cpp \
$(SOURCE)/Model/DynamicMesh.cpp \
$(SOURCE)/Model/DynamicMeshPrivate-GLES2.cpp \
\
$(SOURCE)/Shader/FragmentSource.cpp \
$(SOURCE)/Shader/ShaderManager.cpp \
$(SOURCE)/Shader/ShaderProgram-GLES2.cpp \
$(SOURCE)/Shader/ShaderSource-GLES2.cpp \
$(SOURCE)/Shader/Shader-GLES2.cpp \
$(SOURCE)/Shader/ShaderManagerPrivate.cpp \
$(SOURCE)/Shader/ShaderProgramSymbol-GLES2.cpp \
$(SOURCE)/Shader/VertexSource.cpp \
\
$(SOURCE)/Text/Text.cpp \
$(SOURCE)/Text/RichText.cpp \
$(SOURCE)/Text/FontAsset.cpp \
$(SOURCE)/Text/FontResource.cpp \
$(SOURCE)/Text/FontManager.cpp \
$(SOURCE)/Text/FontManagerPrivate.cpp \
\
$(SOURCE)/kaski/libtess/dict.c \
$(SOURCE)/kaski/libtess/memalloc.c \
$(SOURCE)/kaski/libtess/normal.c \
$(SOURCE)/kaski/libtess/sweep.c \
$(SOURCE)/kaski/libtess/tessmono.c \
$(SOURCE)/kaski/libtess/geom.c \
$(SOURCE)/kaski/libtess/mesh.c \
$(SOURCE)/kaski/libtess/priorityq.c \
$(SOURCE)/kaski/libtess/render.c \
$(SOURCE)/kaski/libtess/tess.c \
\
$(SOURCE)/kaski/font.cpp \
$(SOURCE)/kaski/glyph.cpp \
$(SOURCE)/kaski/glyph_cache.cpp \
$(SOURCE)/kaski/glyph_instance.cpp \
$(SOURCE)/kaski/glyph_matrix.cpp \
$(SOURCE)/kaski/math.cpp \
$(SOURCE)/kaski/text.cpp \
\
$(SOURCE)/Animation/AnimationManager.cpp \
$(SOURCE)/Animation/Animation.cpp \
$(SOURCE)/Animation/AnimationPrivate.cpp \
$(SOURCE)/Animation/BezierComplexPath.cpp \
$(SOURCE)/Animation/BezierComplexPathPrivate.cpp \
$(SOURCE)/Animation/BezierQuadraticPath.cpp \
$(SOURCE)/Animation/BezierQuadraticPathPrivate.cpp \
$(SOURCE)/Animation/ControlPoint.cpp \
$(SOURCE)/Animation/ControlPointPrivate.cpp \
$(SOURCE)/Animation/Easing.cpp \
$(SOURCE)/Animation/LinearPath.cpp \
$(SOURCE)/Animation/LinearPathPrivate.cpp \
$(SOURCE)/Animation/Path.cpp \
\
$(SOURCE)/StateMachine/StateMachine.cpp \
$(SOURCE)/StateMachine/State.cpp \
$(SOURCE)/StateMachine/StateTransition.cpp \
\
$(SOURCE)/Arrange/Grid.cpp \
$(SOURCE)/Arrange/Item.cpp \
$(SOURCE)/Arrange/Move.cpp


# Dependencies
LOCAL_STATIC_LIBRARIES := libzip
LOCAL_SHARED_LIBRARIES := libgrapevine_png libsigcpp libiconv
LOCAL_STATIC_LIBRARY   := libgnustl
LOCAL_LDLIBS           := -llog -lz -lm -landroid -lGLESv2
LOCAL_CFLAGS           := -Wall -g

include $(BUILD_SHARED_LIBRARY)

