#include <jni.h>
#include <dlfcn.h>
#include <android/log.h>
#include <cstring>
#include <cstdio>
#include "zygisk.hpp"

#define LOG_TAG "ZYGISK_TEST"

class CF_ESP : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
        __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "onLoad ejecutado!");
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        const char *process = env->GetStringUTFChars(args->nice_name, nullptr);
        if (process) {
            __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "PostApp: %s", process);
            
            // Intentar crear el archivo de prueba
            FILE *f = fopen("/data/data/com.ScarletWork.CounterFlame/files/zygisk_proof.txt", "w");
            if (f) {
                fprintf(f, "Proceso: %s\n", process);
                fclose(f);
            }

            // Si el proceso es el juego, intentamos inyectar
            if (strstr(process, "CounterFlame")) {
                __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Inyectando ESP...");
                void *handle = dlopen("/data/data/com.ScarletWork.CounterFlame/files/libMyLibName.so", RTLD_NOW);
                if (handle) {
                    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "ESP CARGADO EXITOSAMENTE");
                } else {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "FALLO AL CARGAR: %s", dlerror());
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
