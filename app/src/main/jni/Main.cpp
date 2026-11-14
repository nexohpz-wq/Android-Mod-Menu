#include <jni.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES3/gl3.h>
#include <dlfcn.h>
#include <pthread.h>
#include <unistd.h>
#include <cmath>
#include <set>
#include <algorithm>
#include <cstdio>
#include <mutex>
#include <android/log.h>
#include "Includes/Utils.hpp"
#include "Dobby/dobby.h"

struct Vector3 { float x, y, z; };

uintptr_t offset_PM_Update = 0x29aa3d8;
uintptr_t offset_Camera_main = 0x2b55a10;
uintptr_t offset_W2S = 0x2b55654;
uintptr_t offset_Transform_pos = 0x2946fbc;
uintptr_t offset_Component_transform = 0x293f0d0;

void* (*Camera_get_main)() = nullptr;
Vector3 (*Camera_WorldToScreenPoint)(void* cam, Vector3 pos, int eye) = nullptr;
void* (*Component_get_transform)(void* comp) = nullptr;
Vector3 (*Transform_get_position)(void* trans) = nullptr;

void (*orig_PM_Update)(void* instance) = nullptr;
std::set<void*> active_players;
std::mutex players_mutex;

GLuint shaderProgram = 0;
GLint positionLoc = -1;
GLuint vao = 0;
bool shaderInitialized = false;
void (*glGenVertexArraysOES)(GLsizei, GLuint*) = nullptr;
void (*glBindVertexArrayOES)(GLuint) = nullptr;

// Escribir en archivo para ver si el codigo corre
void WriteLog(const char* msg) {
    FILE *f = fopen("/data/data/com.ScarletWork.CounterFlame/files/esp_debug.txt", "a");
    if (f) { fprintf(f, "%s\n", msg); fclose(f); }
}

void hook_PM_Update(void* instance) {
    if (instance) {
        std::lock_guard<std::mutex> lock(players_mutex);
        if (active_players.size() < 100) active_players.insert(instance);
    }
    if (orig_PM_Update) orig_PM_Update(instance);
}

typedef EGLBoolean (*eglSwapBuffers_t)(EGLDisplay dpy, EGLSurface surf);
eglSwapBuffers_t orig_eglSwapBuffers = nullptr;

typedef EGLBoolean (*eglSwapBuffersWithDamage_t)(EGLDisplay dpy, EGLSurface surf, EGLint* rects, EGLint n_rects);
eglSwapBuffersWithDamage_t orig_eglSwapBuffersWithDamageKHR = nullptr;

GLuint CreateShaderProgram() {
    const char* vSource = "attribute vec4 vPosition; void main() { gl_Position = vPosition; }";
    const char* fSource = "precision mediump float; void main() { gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); }"; // Rojo
    
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER); glShaderSource(vShader, 1, &vSource, NULL); glCompileShader(vShader);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER); glShaderSource(fShader, 1, &fSource, NULL); glCompileShader(fShader);
    GLuint program = glCreateProgram(); glAttachShader(program, vShader); glAttachShader(program, fShader); glLinkProgram(program);
    return program;
}

void DrawTestSquare() {
    if (!shaderInitialized) {
        shaderProgram = CreateShaderProgram();
        positionLoc = glGetAttribLocation(shaderProgram, "vPosition");
        glGenVertexArraysOES = (void(*)(GLsizei, GLuint*))eglGetProcAddress("glGenVertexArraysOES");
        glBindVertexArrayOES = (void(*)(GLuint))eglGetProcAddress("glBindVertexArrayOES");
        if (glGenVertexArraysOES) glGenVertexArraysOES(1, &vao);
        shaderInitialized = true;
        WriteLog("Shader inicializado.");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_SCISSOR_TEST); 
    glDisable(GL_DEPTH_TEST); 
    glDisable(GL_STENCIL_TEST);
    glUseProgram(shaderProgram);
    if (glBindVertexArrayOES) glBindVertexArrayOES(vao);

    // Cuadrado rojo gigante en el centro de la pantalla
    GLfloat vertices[] = {
        -0.5f,  0.5f,   0.5f,  0.5f,  // Arriba
        -0.5f,  0.5f,  -0.5f, -0.5f,  // Izquierda
         0.5f,  0.5f,   0.5f, -0.5f,  // Derecha
        -0.5f, -0.5f,   0.5f, -0.5f   // Abajo
    };

    glVertexAttribPointer(positionLoc, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(positionLoc);
    glDrawArrays(GL_LINES, 0, 8);
    glDisableVertexAttribArray(positionLoc);
    
    if (glBindVertexArrayOES) glBindVertexArrayOES(0);
}

EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surf) {
    DrawTestSquare();
    return orig_eglSwapBuffers(dpy, surf);
}

EGLBoolean hook_eglSwapBuffersWithDamageKHR(EGLDisplay dpy, EGLSurface surf, EGLint* rects, EGLint n_rects) {
    DrawTestSquare();
    if (orig_eglSwapBuffersWithDamageKHR) return orig_eglSwapBuffersWithDamageKHR(dpy, surf, rects, n_rects);
    return EGL_TRUE;
}

void* hack_thread(void*) {
    WriteLog("Hack Thread Iniciado");
    
    void* il2cpp_handle = nullptr;
    while (!il2cpp_handle) {
        il2cpp_handle = dlopen("libil2cpp.so", RTLD_LAZY);
        if (!il2cpp_handle) sleep(1);
    }
    
    uintptr_t base = 0;
    void* il2cpp_init = dlsym(il2cpp_handle, "il2cpp_init");
    Dl_info info;
    if (il2cpp_init && dladdr(il2cpp_init, &info)) {
        base = (uintptr_t)info.dli_fbase;
    }
    
    char buf[128];
    sprintf(buf, "Base libil2cpp: %lx", (unsigned long)base);
    WriteLog(buf);
    
    if (base != 0) {
        Camera_get_main = (void*(*)())(base + offset_Camera_main);
        Camera_WorldToScreenPoint = (Vector3(*)(void*, Vector3, int))(base + offset_W2S);
        Component_get_transform = (void*(*)(void*))(base + offset_Component_transform);
        Transform_get_position = (Vector3(*)(void*))(base + offset_Transform_pos);
        
        void* pmUpdate = (void*)(base + offset_PM_Update);
        int res = DobbyHook(pmUpdate, (void*)hook_PM_Update, (void**)&orig_PM_Update);
        sprintf(buf, "Hook PM_Update res: %d", res);
        WriteLog(buf);
    }
    
    void* egl_handle = dlopen("libEGL.so", RTLD_LAZY);
    if (egl_handle) {
        void* swapFunc = dlsym(egl_handle, "eglSwapBuffers");
        if (swapFunc) {
            int r1 = DobbyHook(swapFunc, (void*)hook_eglSwapBuffers, (void**)&orig_eglSwapBuffers);
            sprintf(buf, "Hook eglSwapBuffers res: %d", r1);
            WriteLog(buf);
        }
        
        void* swapKHR = dlsym(egl_handle, "eglSwapBuffersWithDamageKHR");
        if (swapKHR) {
            int r2 = DobbyHook(swapKHR, (void*)hook_eglSwapBuffersWithDamageKHR, (void**)&orig_eglSwapBuffersWithDamageKHR);
            sprintf(buf, "Hook eglSwapBuffersWithDamageKHR res: %d", r2);
            WriteLog(buf);
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
