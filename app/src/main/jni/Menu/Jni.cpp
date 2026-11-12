#include <jni.h>
#include <string>
#include <sstream>
#include <unistd.h>
#include <pthread.h>
#include "Includes/obfuscate.h"
#include "Includes/Logger.h"
#include "Includes/Utils.hpp"

// 1. Declaramos las funciones con los nombres cortos
jstring GetFeatureList(JNIEnv *env, jobject thiz) {
    return env->NewStringUTF("Skeleton ESP");
}

void Changes(JNIEnv *env, jclass clazz, jobject obj, jint feature, jstring value, jint type, jlong longValue, jboolean boolValue, jstring stringValue) {}

void CheckOverlayPermission(JNIEnv *env, jclass thiz, jobject ctx) {
    LOGI("Check overlay permission");
    jclass native_context = env->GetObjectClass(ctx);
    jmethodID startServiceMethod = env->GetMethodID(native_context, "startService", "(Landroid/content/Intent;)Landroid/content/ComponentName;");
    jclass intentClass = env->FindClass("android/content/Intent");
    jclass launcherClass = env->FindClass("com/android/support/Launcher");
    jmethodID newIntent = env->GetMethodID(intentClass, "<init>", "(Landroid/content/Context;Ljava/lang/Class;)V");
    jobject intent = env->NewObject(intentClass, newIntent, ctx, launcherClass);
    env->CallObjectMethod(ctx, startServiceMethod, intent);
}

// 2. Las registramos con JNI_OnLoad usando sus nombres cortos
extern "C" jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) return -1;
    jclass clazz = env->FindClass("com/android/support/Launcher");
    if (clazz) {
        JNINativeMethod methods[] = {
            {(char*)"GetFeatureList", (char*)"()Ljava/lang/String;", (void*)GetFeatureList},
            {(char*)"Changes", (char*)"(Ljava/lang/Object;ILjava/lang/String;IJZLjava/lang/String;)V", (void*)Changes},
            {(char*)"CheckOverlayPermission", (char*)"(Landroid/content/Context;)V", (void*)CheckOverlayPermission}
        };
        env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0]));
    }
    return JNI_VERSION_1_6;
}
