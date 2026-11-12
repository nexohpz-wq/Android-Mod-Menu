#include <jni.h>
#include <pthread.h>
#include <unistd.h>
#include <android/log.h>
#include "Includes/Utils.hpp"

jstring GetFeatureList(JNIEnv *env, jobject thiz) {
    return env->NewStringUTF("ESP Hack");
}

void Changes(JNIEnv *env, jclass clazz, jobject obj, jint feature, jstring value, jint type, jlong longValue, jboolean boolValue, jstring stringValue) {}

__attribute__((constructor))
void lib_main() {
    // Dejamos esto vacío para que el constructor no intente cargar el menú aún.
}
