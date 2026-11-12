#include <jni.h>
#include "Includes/Includes.h"
#include "Menu/Menu.h"

jstring GetFeatureList(JNIEnv *env, jobject thiz) {
    return env->NewStringUTF("ESP Hack");
}

void Changes(JNIEnv *env, jclass clazz, jobject obj, jint feature, jstring value, jint type, jlong longValue, jboolean boolValue, jstring stringValue) {
    // Aquí va la lógica de los botones del menú (por ahora vacío)
}

__attribute__((constructor))
void lib_main() {
    // Inicia el menú de LGL
}
