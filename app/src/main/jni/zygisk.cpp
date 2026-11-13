#include <jni.h>
#include <dlfcn.h>
#include <android/log.h>
#include <cstring>
#include <cstdio>
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
                // Escribimos el log en la carpeta privada del juego
                FILE *f = fopen("/data/data/com.ScarletWork.CounterFlame/files/zygisk_log.txt", "w");
                if (f) {
                    fprintf(f, "Iniciando inyeccion...\n");
                    // Intentamos cargar el ESP
                    void *handle = dlopen("/data/data/com.ScarletWork.CounterFlame/files/libMyLibName.so", RTLD_NOW);
                    if (handle) {
                        fprintf(f, "EXITO: ESP inyectado.\n");
                    } else {
                        const char *err = dlerror();
                        fprintf(f, "ERROR: %s\n", err ? err : "Desconocido");
                    }
                    fclose(f);
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
