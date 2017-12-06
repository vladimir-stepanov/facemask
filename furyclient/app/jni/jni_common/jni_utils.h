#ifndef JNI_UTILS_H
#define JNI_UTILS_H

#include <jni.h>
#include <string>

namespace jniutils {

char* convertJStrToCStr(JNIEnv* env, jstring lString);

std::string convertJStrToString(JNIEnv* env, jstring lString);

JNIEnv* vm2env(JavaVM* vm);

}  // end jniutils

#endif /* JNI_UTILS_H */
