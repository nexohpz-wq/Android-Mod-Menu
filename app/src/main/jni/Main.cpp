#include <jni.h>
#include <string>

jstring GetFeatureList(JNIEnv* env, jobject thiz) {
    return env->NewStringUTF("ESP Hack");
}

void Changes(JNIEnv* env, jclass clazz, jobject obj, jint feature, jstring value, jint type, jlong longValue, jboolean boolValue, jstring stringValue) {
    // Lógica de los botones del menú (vacío por ahora)
}

__attribute__((constructor))
void lib_main() {
    // Aquí arranca el menú en segundo plano
}
