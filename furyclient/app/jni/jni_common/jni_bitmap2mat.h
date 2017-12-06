#ifndef BITMAP2MAT_H
#define BITMAP2MAT_H

#include <android/bitmap.h>
#include <glog/logging.h>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

namespace jniutils {

void ConvertBitmapToRGBAMat(JNIEnv* env, jobject& bitmap, cv::Mat& dst,
                            bool needUnPremultiplyAlpha);

}  // end jniutils

#endif /* BITMAP2MAT_H */
