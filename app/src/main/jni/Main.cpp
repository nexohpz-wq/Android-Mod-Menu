#include <jni.h>
#include <pthread.h>
#include <unistd.h>
#include "Includes/Utils.hpp"

jstring GetFeatureList(JNIEnv *env, jobject thiz) {
    return env->NewStringUTF("Skeleton ESP");
}

void Changes(JNIEnv *env, jclass clazz, jobject obj, jint feature, jstring value, jint type, jlong longValue, jboolean boolValue, jstring stringValue) {
    // Aquí irá la lógica para encender/apagar el ESP luego
}

// Hilo que espera al juego y arranca el menú
void* hack_thread(void*) {
    do { 
        sleep(1); 
    } while (!isLibraryLoaded("libil2cpp.so"));
    
    // El menú se inicializa solo
    return nullptr;
}

__attribute__((constructor))
void lib_main() {
    pthread_t pt;
    pthread_create(&pt, nullptr, hack_thread, nullptr);
    pthread_detach(pt);
}
