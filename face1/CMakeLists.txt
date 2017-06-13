# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

## Define each subfolders
set(EXT_DIR ../3rdparty)
set(GRAPEVINE_FOUNDATION_DIR ${EXT_DIR}/grapevine-foundation)
set(GRAPEVINE_3DENGINE_DIR ${EXT_DIR}/grapevine-3dengine)
set(LIB_PNG_DIR ${EXT_DIR}/libpng)
set(LIB_JPEG_DIR ${EXT_DIR}/libjpeg)
set(LIB_SIGCPP_DIR ${EXT_DIR}/libsigcpp)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/jni.cpp
             src/main/cpp/MainView.cpp  )

#
# libsigcpp related
# TODO: make a separate CMakeLists.txt
#

include_directories(
    ${GRAPEVINE_FOUNDATION_DIR}/include
    ${GRAPEVINE_3DENGINE_DIR}/include
    ${LIB_PNG_DIR}/jni
    ${LIB_JPEG_DIR}/src
    ${LIB_SIGCPP_DIR}/jni
    ${LIB_SIGCPP_DIR}/functors
    ${LIB_SIGCPP_DIR}/adaptors
    ${LIB_SIGCPP_DIR}/adaptors/lamda
    ${LIB_SIGCPP_DIR} )


#
# libpng related
# TODO: make a separate CMakeLists.txt
#

AUX_SOURCE_DIRECTORY( ${LIB_PNG_DIR}/jni SOURCES_PNG)
message("libpng sources: '${SOURCES_PNG}'")


#
# libjpeg related
# TODO: make a separate CMakeLists.txt
#
AUX_SOURCE_DIRECTORY( ${LIB_JPEG_DIR}/src SOURCES_JPEG)
message("libjpeg sources: '${SOURCES_JPEG}'")


#
# grapevine-foundation related
# TODO: make a separate CMakeLists.txt
#
AUX_SOURCE_DIRECTORY( ${GRAPEVINE_FOUNDATION_DIR}/src/Core SOURCES_CORE)
message("grapevine-foundation Core sources: '${SOURCES_CORE}'")

AUX_SOURCE_DIRECTORY( ${GRAPEVINE_FOUNDATION_DIR}/src/GPU SOURCES_GPU)
message("grapevine-foundation GPU sources: '${SOURCES_GPU}'")

set (SOURCES_CANVAS ${GRAPEVINE_FOUNDATION_DIR}/src/Canvas/Canvas-Android.cpp
                     ${GRAPEVINE_FOUNDATION_DIR}/src/Canvas/CanvasPrivate.cpp )
message("grapevine-foundation Canvas sources: '${SOURCES_CANVAS}'")

set (SOURCES_INPUT ${GRAPEVINE_FOUNDATION_DIR}/src/Input/InputSource-Android.cpp
                     ${GRAPEVINE_FOUNDATION_DIR}/src/Input/InputSourcePrivate.cpp
                     ${GRAPEVINE_FOUNDATION_DIR}/src/Input/SyncedSlideEvent.cpp )
message("grapevine-foundation Input sources: '${SOURCES_INPUT}'")

AUX_SOURCE_DIRECTORY( ${GRAPEVINE_FOUNDATION_DIR}/src/Math SOURCES_MATH)
message("grapevine-foundation Math sources: '${SOURCES_MATH}'")

AUX_SOURCE_DIRECTORY( ${GRAPEVINE_FOUNDATION_DIR}/src/Resource SOURCES_RESOURCE)
message("grapevine-foundation Resource sources: '${SOURCES_RESOURCE}'")

set (SOURCES_INPUT ${GRAPEVINE_FOUNDATION_DIR}/src/Image/Bitmap.cpp
                     ${GRAPEVINE_FOUNDATION_DIR}/src/Image/ImageAsset.cpp
                     ${GRAPEVINE_FOUNDATION_DIR}/src/Image/ImagePng.cpp
                     ${GRAPEVINE_FOUNDATION_DIR}/src/Image/GeneralImageResource.cpp
                     ${GRAPEVINE_FOUNDATION_DIR}/src/Image/ImageResource.cpp )
message("grapevine-foundation Image sources: '${SOURCES_IMAGE}'")

AUX_SOURCE_DIRECTORY( ${GRAPEVINE_FOUNDATION_DIR}/src/StateMachine SOURCES_STATEMACHINE)
message("grapevine-foundation StateMachine sources: '${SOURCES_STATEMACHINE}'")

AUX_SOURCE_DIRECTORY( ${GRAPEVINE_FOUNDATION_DIR}/src/Animation SOURCES_ANIMATION)
message("grapevine-foundation Animation sources: '${SOURCES_ANIMATION}'")

AUX_SOURCE_DIRECTORY( ${GRAPEVINE_FOUNDATION_DIR}/src/Material SOURCES_MATERIAL)
message("grapevine-foundation Material sources: '${SOURCES_MATERIAL}'")

AUX_SOURCE_DIRECTORY( ${GRAPEVINE_FOUNDATION_DIR}/src/Benchmark SOURCES_BENCHMARK)
message("grapevine-foundation Benchmark sources: '${SOURCES_BENCHMARK}'")

AUX_SOURCE_DIRECTORY( ${GRAPEVINE_FOUNDATION_DIR}/src/Arrange SOURCES_ARRANGE)
message("grapevine-foundation Arrange sources: '${SOURCES_ARRANGE}'")

AUX_SOURCE_DIRECTORY( ${GRAPEVINE_FOUNDATION_DIR}/src/Model SOURCES_MODEL)
message("grapevine-foundation Model sources: '${SOURCES_MODEL}'")

AUX_SOURCE_DIRECTORY( ${GRAPEVINE_FOUNDATION_DIR}/src/Shader SOURCES_SHADER)
message("grapevine-foundation Shader sources: '${SOURCES_SHADER}'")

add_library( # Sets the name of the library.
    grapevine-foundation

    # Sets the library as a shared library.
    SHARED

    # Provides a relative path to your source file(s).
    ${LIB_SIGCPP_DIR}/jni/sigc++/signal.cc
    ${LIB_SIGCPP_DIR}/jni/sigc++/signal_base.cc
    ${LIB_SIGCPP_DIR}/jni/sigc++/connection.cc
    ${LIB_SIGCPP_DIR}/jni/sigc++/trackable.cc
    ${LIB_SIGCPP_DIR}/jni/sigc++/functors/slot_base.cc
    ${LIB_SIGCPP_DIR}/jni/sigc++/functors/slot.cc
    ${LIB_SIGCPP_DIR}/jni/sigc++/adaptors/lambda/lambda.cc

    ${SOURCES_PNG}
    ${SOURCES_JPEG}

    ${SOURCES_CORE}
    ${SOURCES_GPU}
    ${SOURCES_CANVAS}
    ${SOURCES_INPUT}
    ${SOURCES_MATH}
    ${SOURCES_RESOURCE}
    ${SOURCES_IMAGE}
    ${SOURCES_STATEMACHINE}
    ${SOURCES_ANIMATION}
    ${SOURCES_MATERIAL}
    ${SOURCES_BENCHMARK}
    ${SOURCES_ARRANGE}
    ${SOURCES_MODEL}
    ${SOURCES_SHADER}
)

#
# grapevine-foundation related
# TODO: make a separate CMakeLists.txt
#

AUX_SOURCE_DIRECTORY( ${GRAPEVINE_3DENGINE_DIR}/src/Scene SOURCES_SCENE)
message("grapevine-3dengine Scene sources: '${SOURCES_SCENE}'")

AUX_SOURCE_DIRECTORY( ${GRAPEVINE_3DENGINE_DIR}/src/SceneExtensions SOURCES_SCENEEXTENSIONS)
message("grapevine-3dengine SceneExtensions sources: '${SOURCES_SCENEEXTENSIONS}'")

add_library( # Sets the name of the library.
    grapevine-3dengine

    SHARED

    ${SOURCES_SCENE}
    ${SOURCES_SCENEEXTENSIONS}
)


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries(grapevine-foundation log z m android GLESv2)
target_link_libraries(grapevine-3dengine grapevine-foundation log z m android GLESv2)

target_link_libraries( # Specifies the target library.
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                    ${log-lib}
                    grapevine-foundation
                    grapevine-3dengine)

