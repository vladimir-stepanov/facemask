
CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckIncludeFileCXX)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckCXXCompilerFlag)

SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH} .")

PROJECT(GRAPEVINEFOUNDATION)

IF (CMAKE_TOOLCHAIN_FILE)
    INCLUDE(${CMAKE_TOOLCHAIN_FILE})
ENDIF ()

FIND_PACKAGE(PkgConfig)

SET(DESKTOPLINUX OFF CACHE BOOL "Desktop Linux support")
SET(DESKTOPLINUX_XEGL_GLES2 OFF CACHE BOOL "Desktop Linux support with XEGL/GLES2")
SET(NATIVE_EGL_GLES2 OFF CACHE BOOL "Native EGL/GLES2 support")
SET(GL_HPP OFF CACHE BOOL "Use GL.hpp")
SET(USE_GLES2 OFF CACHE BOOL "Use GLES2")
SET(USE_XINPUT2 OFF CACHE BOOL "Use Xinput2")

IF (DESKTOPLINUX MATCHES ON)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDESKTOPLINUX_XEGL_GLES2")
    SET(GL_HPP ON)
    SET(USE_GLES2 ON)
    SET(USE_XINPUT2 ON)
ENDIF ()

IF (NATIVE_EGL_GLES2 MATCHES ON)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNATIVE_EGL_GLES2")
    SET(GL_HPP ON)
    SET(USE_GLES2 ON)
    SET(USE_XINPUT2 OFF)
ENDIF ()

IF (DESKTOPLINUX MATCHES ON OR NATIVE_EGL_GLES2 MATCHES ON)
    PKG_CHECK_MODULES(DEPENDENCIES REQUIRED glib-2.0>=2.24 sigc++-2.0>=2.2 glesv2 egl libpng12 libjpeg)
    INCLUDE_DIRECTORIES(${DEPENDENCIES_INCLUDE_DIRS})
ENDIF ()

IF (ANDROID MATCHES ON)
    PKG_CHECK_MODULES(DEPENDENCIES REQUIRED sigc++-2.0 ngdisplay)
    INCLUDE_DIRECTORIES(${DEPENDENCIES_INCLUDE_DIRS})
ENDIF ()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall -Wextra")

SET(GRAPEVINEFOUNDATION_SRC
    src/Animation/Path.cpp ;
    src/Animation/BezierQuadraticPathPrivate.cpp ;
    src/Animation/BezierQuadraticPath.cpp ;
    src/Animation/BezierComplexPath.cpp ;
    src/Animation/BezierComplexPathPrivate.cpp ;
    src/Animation/LinearPathPrivate.cpp ;
    src/Animation/LinearPath.cpp ;
    src/Animation/Easing.cpp ;
    src/Animation/AnimationManager.cpp ;
    src/Animation/AnimationPrivate.cpp ;
    src/Animation/Animation.cpp ;
    src/Animation/ControlPoint.cpp ;
    src/Animation/ControlPointPrivate.cpp ;

    src/Arrange/Grid.cpp ;
    src/Arrange/Item.cpp ;
    src/Arrange/Move.cpp ;

    src/Benchmark/FpsCounter.cpp ;
    src/Benchmark/DualScrollTypeHwcSwapsFpsLogger.cpp ;

    src/Canvas/CanvasPrivate.cpp ;

    src/Core/Log.cpp ;
    src/Core/Timer.cpp ;
    src/Core/Mutex.cpp ;
    src/Core/RecursiveMutex.cpp ;
    src/Core/Condition.cpp ;
    src/Core/Thread.cpp ;
    src/Core/Referenced.cpp

    src/Image/ImageAsset.cpp ;
    src/Image/ImageResource.cpp ;
    src/Image/ImagePng.cpp ;
    src/Image/ImageJpeg.cpp ;
    src/Image/ImageKtx.cpp ;
    src/Image/GeneralImageResource.cpp ;

    src/Input/InputSourcePrivate.cpp ;

    src/Material/TextureAsset.cpp ;
    src/Material/CubemapAsset.cpp ;
    src/Material/MaterialAsset.cpp ;
    src/Material/MaterialResource.cpp ;
    src/Material/FramebufferAsset.cpp ;

    src/Math/Matrix.cpp ;
    src/Math/Vector.cpp ;
    src/Math/Plane.cpp ;
    src/Math/Ray.cpp ;
    src/Math/Sphere.cpp ;
    src/Math/Box.cpp ;
    src/Math/Cylinder.cpp ;

    src/Model/ModelAsset.cpp ;
    src/Model/MeshAsset.cpp ;
    src/Model/ModelResource.cpp ;
    src/Model/ModelFactory.cpp ;
    src/Model/ModelData.cpp ;
    src/Model/Barrel.cpp ;
    src/Model/DynamicMesh.cpp ;
    src/Model/DynamicMeshPrivate-GLES2.cpp ;

    src/Resource/File.cpp ;
    src/Resource/Resource.cpp ;
    src/Resource/ResourceManager.cpp ;
    src/Resource/ResourceManagerPrivate.cpp ;
    src/Resource/MemoryFile.cpp ;

    src/Shader/ShaderManager.cpp ;
    src/Shader/ShaderManagerPrivate.cpp ;
    src/Shader/FragmentSource.cpp ;
    src/Shader/VertexSource.cpp ;

    src/StateMachine/State.cpp ;
    src/StateMachine/StateTransition.cpp ;
    src/StateMachine/StateMachine.cpp ;

    )

IF (DESKTOPLINUX_XEGL_GLES2 MATCHES ON)
    SET(GRAPEVINEFOUNDATION_SRC
        ${GRAPEVINEFOUNDATION_SRC}
        src/Canvas/Canvas-XEGL.cpp ;
        src/Canvas/CanvasPrivateXEGL.cpp ;
        )
ENDIF ()

IF (NATIVE_EGL_GLES2 MATCHES ON OR ANDROID MATCHES ON)
    SET(GRAPEVINEFOUNDATION_SRC
        ${GRAPEVINEFOUNDATION_SRC}
        src/Canvas/Canvas-EGL.cpp ;
        )
ENDIF ()

IF (GL_HPP MATCHES ON OR ANDROID MATCHES ON)
    SET(GRAPEVINEFOUNDATION_SRC
        ${GRAPEVINEFOUNDATION_SRC}
        src/GPU/GL.cpp ;
        )
ENDIF ()

IF (USE_GLES2 MATCHES ON OR ANDROID MATCHES ON)
    SET(GRAPEVINEFOUNDATION_SRC
        ${GRAPEVINEFOUNDATION_SRC}
        src/Material/TextureResource-GLES2.cpp ;
        src/Material/CubemapResource-GLES2.cpp ;
        src/Model/MeshResource-GLES2.cpp ;
        src/Shader/Shader-GLES2.cpp ;
        src/Shader/ShaderProgram-GLES2.cpp ;
        src/Shader/ShaderSource-GLES2.cpp ;
        src/Shader/ShaderProgramSymbol-GLES2.cpp ;
        )
ENDIF ()


IF (USE_XINPUT2 MATCHES ON)
    SET(GRAPEVINEFOUNDATION_SRC
        ${GRAPEVINEFOUNDATION_SRC}
        src/Input/InputSourcePrivate.cpp ;
        src/Input/InputSource-XDisplay.cpp ;
        src/Input/InputSourcePrivateXDisplay.cpp ;
        src/Input/SyncedSlideEvent.cpp ;
        )
ENDIF ()

# Currently the Mate8 input method is compiled for Android
IF (ANDROID MATCHES ON)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPOWER_DEV=\\\"/dev/input/event1\\\"")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVOLUME_DEV=\\\"/dev/input/event0\\\"")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTOUCH_DEV=\\\"/dev/input/event4\\\"")


    SET(GRAPEVINEFOUNDATION_SRC
        ${GRAPEVINEFOUNDATION_SRC}
        src/Input/InputSource-NGOS.cpp ;
        src/Input/InputSourcePrivateNGOS.cpp ;
        src/Input/InputSourcePrivateNGOS-Grace.cpp ;
        src/Model/DynamicMeshPrivate-GLES2.cpp ;
        )
ENDIF ()

INCLUDE_DIRECTORIES(../include)

ADD_LIBRARY(grapevine-foundation SHARED ${GRAPEVINEFOUNDATION_SRC})
TARGET_LINK_LIBRARIES(grapevine-foundation ${DEPENDENCIES_LDFLAGS} ${ANDROID_SHARED_LINKER_FLAGS})
IF (ANDROID MATCHES ON)
    TARGET_LINK_LIBRARIES(grapevine-foundation -lpng -ljpeg -landroid)
ENDIF ()

# Add the correct include directory for the local Android builds
IF (ANDROID MATCHES ON)
    SET(includedir ${CMAKE_INSTALL_PREFIX}/include/)
    SET(libdir ${CMAKE_INSTALL_PREFIX}/lib64/)
ENDIF ()
IF (NOT ANDROID MATCHES ON)
    SET(libdir /usr/lib -lpthread -lrt)
ENDIF ()
CONFIGURE_FILE(grapevine-foundation.pc.in grapevine-foundation.pc)
CONFIGURE_FILE(grapevine-foundation-xegl_gles2.pc.in grapevine-foundation-xegl_gles2.pc)

INSTALL(DIRECTORY ../include/Common DESTINATION ${CMAKE_INSTALL_PREFIX}/include/)
INSTALL(DIRECTORY ../include/Grapevine DESTINATION ${CMAKE_INSTALL_PREFIX}/include/)
INSTALL(TARGETS grapevine-foundation LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
INSTALL(FILES grapevine-foundation.pc grapevine-foundation-xegl_gles2.pc
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
INSTALL(FILES src/Resource/ResourceManagerPrivate.hpp
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/Grapevine/Resource)

