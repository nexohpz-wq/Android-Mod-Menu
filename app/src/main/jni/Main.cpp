#include <jni.h>
#include <string>
#include <thread>
#include <chrono>
#include <android/log.h>
#include <dlfcn.h>
#include "Includes/Utils.hpp"

#define LOG_TAG "ESP_MOD"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// Estas dos funciones son las que el menú de Java necesita para funcionar
jstring GetFeatureList(JNIEnv* env, jobject thiz) {
    return env->NewStringUTF("Skeleton ESP");
}

void Changes(JNIEnv* env, jclass clazz, jobject obj, jint feature, jstring value, jint type, jlong longValue, jboolean boolValue, jstring stringValue) {
    // Aquí iría la lógica de cuando tocas el botón del menú
}

// Tu función para el ESP
void hack_thread() {
    while (!isLibraryLoaded("libil2cpp.so")) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    LOGI("Mod Menu Iniciado");
    // Aquí en el futuro irá tu hook
}

__attribute__((constructor))
void lib_main() {
    std::thread(hack_thread).detach();
}
