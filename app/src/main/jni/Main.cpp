#include <jni.h>
#include <thread>
#include <chrono>
#include <android/log.h>
#include <dlfcn.h>

#define LOG_TAG "ESP_MOD"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

bool isLibraryLoaded(const char* libName) {
    void* handle = dlopen(libName, RTLD_LAZY);
    if (handle) { dlclose(handle); return true; }
    return false;
}

void hack_thread() {
    do { sleep(1); } while (!isLibraryLoaded("libil2cpp.so"));
    LOGI("Mod Menu Iniciado");
}

__attribute__((constructor))
void lib_main() {
    std::thread(hack_thread).detach();
}
