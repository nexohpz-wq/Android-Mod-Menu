#include <jni.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <dlfcn.h>
#include <android/log.h>
#include <pthread.h>
#include <unistd.h>
#include <cmath>
#include "Includes/Utils.hpp"
#include "Dobby/dobby.h"

#define TAG "ESP_NATIVE"

// ==========================================
// AQUÍ IRÁN LOS OFFSETS DEL JUEGO
// ==========================================
uintptr_t offset_WorldToScreenPoint = 0x0;
uintptr_t offset_Camera_get_main = 0x0;
uintptr_t offset_PlayerMovement_Update = 0x0;

typedef EGLBoolean (*eglSwapBuffers_t)(EGLDisplay dpy, EGLSurface surf);
eglSwapBuffers_t orig_eglSwapBuffers;

void DrawLine(float x1, float y1, float x2, float y2, float width) {
    GLfloat vertices[] = { x1, y1, x2, y2 };
    glLineWidth(width);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_LINES, 0, 2);
}

EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surf) {
    DrawLine(100.0f, 100.0f, 500.0f, 500.0f, 3.0f);
    return orig_eglSwapBuffers(dpy, surf);
}

void* hack_thread(void*) {
    do { sleep(1); } while (!isLibraryLoaded("libil2cpp.so"));

    __android_log_print(ANDROID_LOG_INFO, TAG, "Juego cargado. Iniciando ESP...");

    void* egl_handle = dlopen("libEGL.so", RTLD_LAZY);
    if (egl_handle) {
        void* swapFunc = dlsym(egl_handle, "eglSwapBuffers");
        if (swapFunc) {
            DobbyHook(swapFunc, (void*)hook_eglSwapBuffers, (void**)&orig_eglSwapBuffers);
            __android_log_print(ANDROID_LOG_INFO, TAG, "Hook EGL exitoso.");
        }
    }
    return nullptr;
}

__attribute__((constructor))
void lib_main() {
    pthread_t pt;
    pthread_create(&pt, nullptr, hack_thread, nullptr);
    pthread_detach(pt);
}
