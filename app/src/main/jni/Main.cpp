#include <jni.h>
#include "Includes/Utils.hpp"

jstring GetFeatureList(JNIEnv* env, jobject thiz) {
    return env->NewStringUTF("ESP Hack");
}

void Changes(JNIEnv* env, jclass clazz, jobject obj, jint feature, jstring value, jint type, jlong longValue, jboolean boolValue, jstring stringValue) {}

__attribute__((constructor))
void lib_main() {}
