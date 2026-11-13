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

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        // En preAppSpecialize somos root, podemos escribir en /data/local/tmp sin problemas
        const char *process = env->GetStringUTFChars(args->nice_name, nullptr);
        if (process) {
            FILE *f = fopen("/data/local/tmp/zygisk_pre.txt", "a");
            if (f) {
                fprintf(f, "PreApp: %s\n", process);
                fclose(f);
            }
            env->ReleaseStringUTFChars(args->nice_name, process);
        }
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        const char *process = env->GetStringUTFChars(args->nice_name, nullptr);
        if (process) {
            // Escribimos en la carpeta del juego sin importar qué juego sea, para ver si tenemos permiso
            FILE *f = fopen("/data/data/com.ScarletWork.CounterFlame/files/post_debug.txt", "a");
            if (f) {
                fprintf(f, "PostApp: %s\n", process);
                // Si el proceso es el de nuestro juego, intentamos inyectar
                if (strstr(process, "CounterFlame")) {
                    fprintf(f, "Encontrado CounterFlame! Intentando cargar ESP...\n");
                    void *handle = dlopen("/data/data/com.ScarletWork.CounterFlame/files/libMyLibName.so", RTLD_NOW);
                    if (handle) {
                        fprintf(f, "EXITO: ESP cargado en memoria!\n");
                    } else {
                        fprintf(f, "ERROR al cargar ESP: %s\n", dlerror());
                    }
                }
                fclose(f);
            }
            env->ReleaseStringUTFChars(args->nice_name, process);
        }
    }
private:
    zygisk::Api *api = nullptr;
    JNIEnv *env = nullptr;
};

REGISTER_ZYGISK_MODULE(CF_ESP)
