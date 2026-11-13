#include <jni.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <dlfcn.h>
#include <unistd.h>
#include <cmath>
#include <android/log.h>
#include "dobby.h"

typedef EGLBoolean (*eglSwapBuffers_t)(EGLDisplay dpy, EGLSurface surf);
eglSwapBuffers_t orig_eglSwapBuffers = nullptr;

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

// Función pública que Frida llamará
extern "C" void StartESP() {
    __android_log_print(ANDROID_LOG_INFO, "ESP_NATIVE", "StartESP ejecutado desde Frida!");
    void* egl_handle = dlopen("libEGL.so", RTLD_LAZY);
    if (egl_handle) {
        void* swapFunc = dlsym(egl_handle, "eglSwapBuffers");
        if (swapFunc) {
            DobbyHook(swapFunc, (void*)hook_eglSwapBuffers, (void**)&orig_eglSwapBuffers);
            __android_log_print(ANDROID_LOG_INFO, "ESP_NATIVE", "Hook EGL exitoso.");
        }
    }
}
