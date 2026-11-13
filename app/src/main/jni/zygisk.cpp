#include <jni.h>
#include <dlfcn.h>
#include <android/log.h>
#include <cstring>
#include "zygisk.hpp"

class CF_ESP : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        const char *process = env->GetStringUTFChars(args->nice_name, nullptr);
        if (process) {
            if (strcmp(process, "com.ScarletWork.CounterFlame") == 0) {
                // Inyecta tu librería ESP desde la carpeta del módulo
                void *handle = dlopen("/data/adb/modules/cf_esp/lib/arm64-v8a/libMyLibName.so", RTLD_NOW);
                if (handle) {
                    __android_log_print(ANDROID_LOG_INFO, "ZYGISK_ESP", "ESP inyectado con exito en CounterFlame");
                } else {
                    __android_log_print(ANDROID_LOG_ERROR, "ZYGISK_ESP", "Fallo al inyectar: %s", dlerror());
                }
            }
            env->ReleaseStringUTFChars(args->nice_name, process);
        }
    }
private:
    zygisk::Api *api = nullptr;
    JNIEnv *env = nullptr;
};

REGISTER_ZYGISK_MODULE(CF_ESP)
