cmake_minimum_required(VERSION 3.4.1)

message("Checking CMAKE_SYSTEM_NAME = '${CMAKE_SYSTEM_NAME}'")
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  add_definitions(-DOS_OSX)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_definitions(-DOS_LINUX)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  add_definitions(-DOS_WIN)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Android")
  add_definitions(-DOS_ANDROID)
  message("Checking CMAKE_ABI_NAME = '${CMAKE_ANDROID_ARCH_ABI}'")
else()
  message("OS not detected.")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror")

## Define each subfolders
set(TRACKER_SRC jni/tracker)
set(JNI_COMMON_INCLUDE jni)
set(JNI_COMMON_SRC jni/jni_common)
set(EXT_DIR ../3rdparty)
set(GLOG_INCLUDE_DIR ${EXT_DIR}/miniglog)
set(OPENCV_PREBUILT ${EXT_DIR}/opencv/jni)

# Opencv and it will use static import
set(ANDROID_NDK_ABI_NAME ${CMAKE_ANDROID_ARCH_ABI})
include(${OPENCV_PREBUILT}/OpenCVConfig.cmake)

# Include headers
include_directories(${GLOG_INCLUDE_DIR} ${JNI_COMMON_INCLUDE} ${TRACKER_SRC} include)

add_library(tracker_jni SHARED
            ${TRACKER_SRC}/tracker.cpp
            ${TRACKER_SRC}/jni_imageutils.cpp
            ${JNI_COMMON_SRC}/rgb2yuv.cpp
            ${JNI_COMMON_SRC}/yuv2rgb.cpp
            ${JNI_COMMON_SRC}/jni_bitmap2mat.cpp
            ${JNI_COMMON_SRC}/jni_utils.cpp
            ${EXT_DIR}/miniglog/glog/logging.cc)

target_link_libraries(tracker_jni
                      android
                      jnigraphics
                      opencv_imgproc
                      opencv_core
                      opencv_objdetect
                      opencv_calib3d
                      opencv_tracking
                      log)

