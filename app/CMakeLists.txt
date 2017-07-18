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
set(LBP_DETECTION_SRC jni/lbp_detections)
set(HAAR_DETECTION_SRC jni/haar_detections)
set(DLIB_DETECTION_INCLUDE jni/dlib_detections)
set(DLIB_DETECTION_SRC jni/dlib_detections)
set(DLIB_MOD_DETECTION_SRC jni/dlib_mod_detections)
set(JNI_COMMON_INCLUDE jni)
set(JNI_COMMON_SRC jni/jni_common)
set(EXT_DIR ../3rdparty)
set(DLIB_DIR ${EXT_DIR}/dlib)
set(GLOG_INCLUDE_DIR ${EXT_DIR}/miniglog)
set(OPENCV_PREBUILT ${EXT_DIR}/opencv/sdk/native/jni)

set(SEETAFACE_DETECTION_SRC jni/seetaface_detections)
set(SEETAFACE_ALIGNMENT ${EXT_DIR}/seetaface-engine/FaceAlignment)
set(SEETAFACE_DETECTION ${EXT_DIR}/seetaface-engine/FaceDetection)

# Opencv and it will use static import
set(ANDROID_NDK_ABI_NAME ${CMAKE_ANDROID_ARCH_ABI})
include(${OPENCV_PREBUILT}/OpenCVConfig.cmake)

# Include headers
include_directories(${DLIB_DIR} ${GLOG_INCLUDE_DIR} ${JNI_COMMON_INCLUDE} ${DLIB_DETECTION_INCLUDE} include)
include_directories(${SEETAFACE_ALIGNMENT}/include ${SEETAFACE_DETECTION}/include)

add_library(dlib_jni SHARED
            ${DLIB_DETECTION_SRC}/jni_face_det.cpp
            ${DLIB_DETECTION_SRC}/jni_imageutils.cpp
            ${JNI_COMMON_SRC}/jni_bitmap2mat.cpp
            ${JNI_COMMON_SRC}/jni_fileutils.cpp
            ${JNI_COMMON_SRC}/jni_utils.cpp
            ${JNI_COMMON_SRC}/rgb2yuv.cpp
            ${JNI_COMMON_SRC}/yuv2rgb.cpp
            ${DLIB_DIR}/dlib/threads/threads_kernel_shared.cpp
            ${DLIB_DIR}/dlib/entropy_decoder/entropy_decoder_kernel_2.cpp
            ${DLIB_DIR}/dlib/base64/base64_kernel_1.cpp
            ${DLIB_DIR}/dlib/threads/threads_kernel_1.cpp
            ${DLIB_DIR}/dlib/threads/threads_kernel_2.cpp
            ${EXT_DIR}/miniglog/glog/logging.cc)

add_library(dlib_mod_jni SHARED
            ${DLIB_MOD_DETECTION_SRC}/jni_face_det_mod.cpp
            ${DLIB_DETECTION_SRC}/jni_imageutils.cpp
            ${JNI_COMMON_SRC}/jni_bitmap2mat.cpp
            ${JNI_COMMON_SRC}/jni_fileutils.cpp
            ${JNI_COMMON_SRC}/jni_utils.cpp
            ${JNI_COMMON_SRC}/rgb2yuv.cpp
            ${JNI_COMMON_SRC}/yuv2rgb.cpp
            ${DLIB_DIR}/dlib/threads/threads_kernel_shared.cpp
            ${DLIB_DIR}/dlib/entropy_decoder/entropy_decoder_kernel_2.cpp
            ${DLIB_DIR}/dlib/base64/base64_kernel_1.cpp
            ${DLIB_DIR}/dlib/threads/threads_kernel_1.cpp
            ${DLIB_DIR}/dlib/threads/threads_kernel_2.cpp
            ${EXT_DIR}/miniglog/glog/logging.cc)

add_library(haar_jni SHARED
            ${HAAR_DETECTION_SRC}/haar_face_detector.cpp
            ${JNI_COMMON_SRC}/jni_bitmap2mat.cpp
            ${JNI_COMMON_SRC}/jni_utils.cpp
            ${EXT_DIR}/miniglog/glog/logging.cc)

add_library(lbp_jni SHARED
            ${LBP_DETECTION_SRC}/lbp_face_detector.cpp
            ${JNI_COMMON_SRC}/jni_bitmap2mat.cpp
            ${JNI_COMMON_SRC}/jni_utils.cpp
            ${EXT_DIR}/miniglog/glog/logging.cc)


add_library(seetaface_jni SHARED
            ${SEETAFACE_ALIGNMENT}/src/cfan.cpp
            ${SEETAFACE_ALIGNMENT}/src/face_alignment.cpp
            ${SEETAFACE_ALIGNMENT}/src/sift.cpp
            ${SEETAFACE_DETECTION}/src/util/nms.cpp
            ${SEETAFACE_DETECTION}/src/util/image_pyramid.cpp
            ${SEETAFACE_DETECTION}/src/io/lab_boost_model_reader.cpp
            ${SEETAFACE_DETECTION}/src/io/surf_mlp_model_reader.cpp
            ${SEETAFACE_DETECTION}/src/feat/lab_feature_map.cpp
            ${SEETAFACE_DETECTION}/src/feat/surf_feature_map.cpp
            ${SEETAFACE_DETECTION}/src/classifier/lab_boosted_classifier.cpp
            ${SEETAFACE_DETECTION}/src/classifier/mlp.cpp
            ${SEETAFACE_DETECTION}/src/classifier/surf_mlp.cpp
            ${SEETAFACE_DETECTION}/src/face_detection.cpp
            ${SEETAFACE_DETECTION}/src/fust.cpp
            ${SEETAFACE_DETECTION_SRC}/seetaface_detector.cpp
            ${JNI_COMMON_SRC}/jni_bitmap2mat.cpp
            ${JNI_COMMON_SRC}/jni_utils.cpp
            ${DLIB_DIR}/dlib/threads/threads_kernel_shared.cpp
            ${DLIB_DIR}/dlib/entropy_decoder/entropy_decoder_kernel_2.cpp
            ${DLIB_DIR}/dlib/base64/base64_kernel_1.cpp
            ${DLIB_DIR}/dlib/threads/threads_kernel_1.cpp
            ${DLIB_DIR}/dlib/threads/threads_kernel_2.cpp
            ${EXT_DIR}/miniglog/glog/logging.cc)

target_link_libraries(dlib_jni
                      android
                      jnigraphics
                      opencv_imgproc
                      opencv_core
                      opencv_objdetect
                      opencv_calib3d
                      log)

target_link_libraries(dlib_mod_jni
                      android
                      jnigraphics
                      opencv_imgproc
                      opencv_core
                      opencv_objdetect
                      opencv_calib3d
                      log)

target_link_libraries(haar_jni
                      android
                      jnigraphics
                      opencv_imgproc
                      opencv_core
                      opencv_objdetect
                      opencv_calib3d
                      log)

target_link_libraries(lbp_jni
                      android
                      jnigraphics
                      opencv_imgproc
                      opencv_core
                      opencv_objdetect
                      opencv_calib3d
                      log)

target_link_libraries(seetaface_jni
                      android
                      jnigraphics
                      opencv_imgproc
                      opencv_core
                      opencv_objdetect
                      opencv_calib3d
                      log)

