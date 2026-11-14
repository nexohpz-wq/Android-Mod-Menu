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

void WriteLog(const char* msg) {
    FILE *f = fopen("/data/data/com.ScarletWork.CounterFlame/files/esp_debug.txt", "a");
    if (f) { fprintf(f, "%s\n", msg); fclose(f); }
}

void hook_PM_Update(void* instance) {
    static int count = 0;
    if (count++ % 120 == 0) WriteLog("PlayerMovement.Update ejecutándose!"); // Log para saber si avanza
    
    if (instance) {
        std::lock_guard<std::mutex> lock(players_mutex);
        if (active_players.size() < 100) active_players.insert(instance);
    }
    if (orig_PM_Update) orig_PM_Update(instance);
}

typedef EGLBoolean (*eglSwapBuffers_t)(EGLDisplay dpy, EGLSurface surf);
eglSwapBuffers_t orig_eglSwapBuffers = nullptr;

typedef void* (*eglGetProcAddress_t)(const char *procname);
eglGetProcAddress_t orig_eglGetProcAddress = nullptr;

typedef void (*glDrawArraysInstanced_t)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
glDrawArraysInstanced_t orig_glDrawArraysInstanced = nullptr;

GLuint CreateShaderProgram() {
    const char* vSource = "attribute vec4 vPosition; void main() { gl_Position = vPosition; }";
    const char* fSource = "precision mediump float; void main() { gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); }";
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER); glShaderSource(vShader, 1, &vSource, NULL); glCompileShader(vShader);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER); glShaderSource(fShader, 1, &fSource, NULL); glCompileShader(fShader);
    GLuint program = glCreateProgram(); glAttachShader(program, vShader); glAttachShader(program, fShader); glLinkProgram(program);
    return program;
}

void DrawTestSquare() {
    static bool logged = false;
    if (!logged) { WriteLog("DrawTestSquare ejecutado en el hilo de OpenGL!"); logged = true; }

    if (!shaderInitialized) {
        shaderProgram = CreateShaderProgram();
        positionLoc = glGetAttribLocation(shaderProgram, "vPosition");
        glGenVertexArraysOES = (void(*)(GLsizei, GLuint*))eglGetProcAddress("glGenVertexArraysOES");
        glBindVertexArrayOES = (void(*)(GLuint))eglGetProcAddress("glBindVertexArrayOES");
        if (glGenVertexArraysOES) glGenVertexArraysOES(1, &vao);
        shaderInitialized = true;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_SCISSOR_TEST); glDisable(GL_DEPTH_TEST); glDisable(GL_STENCIL_TEST);
    glUseProgram(shaderProgram);
    if (glBindVertexArrayOES) glBindVertexArrayOES(vao);

    GLfloat vertices[] = { -0.5f, 0.5f, 0.5f, 0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f };

    glVertexAttribPointer(positionLoc, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(positionLoc);
    glDrawArrays(GL_LINES, 0, 8);
    glDisableVertexAttribArray(positionLoc);
    if (glBindVertexArrayOES) glBindVertexArrayOES(0);
}

EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surf) {
    WriteLog("hook_eglSwapBuffers llamado!");
    DrawTestSquare();
    return orig_eglSwapBuffers(dpy, surf);
}

void hook_glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instancecount) {
    if (orig_glDrawArraysInstanced) orig_glDrawArraysInstanced(mode, first, count, instancecount);
    
    static int drawCount = 0;
    if (drawCount++ == 100) {
        WriteLog("hook_glDrawArraysInstanced disparado!");
        DrawTestSquare();
    }
}

void* hook_eglGetProcAddress(const char *procname) {
    if (procname) {
        if (strcmp(procname, "glDrawArraysInstanced") == 0) {
            WriteLog("Unity pidió glDrawArraysInstanced!");
            if (!orig_glDrawArraysInstanced) orig_glDrawArraysInstanced = (glDrawArraysInstanced_t)orig_eglGetProcAddress(procname);
            return (void*)hook_glDrawArraysInstanced;
        }
    }
    return orig_eglGetProcAddress(procname);
}

void* hack_thread(void*) {
    WriteLog("hack_thread iniciado");
    
    uintptr_t base = 0;
    while (base == 0) {
        base = getLibraryAddress("libil2cpp.so");
        if (base == 0) sleep(1);
    }
    
    char buf[64];
    sprintf(buf, "Base libil2cpp: 0x%lx", (unsigned long)base);
    WriteLog(buf);
    
    if (base != 0) {
        Camera_get_main = (void*(*)())(base + offset_Camera_main);
        Camera_WorldToScreenPoint = (Vector3(*)(void*, Vector3, int))(base + offset_W2S);
        Component_get_transform = (void*(*)(void*))(base + offset_Component_transform);
        Transform_get_position = (Vector3(*)(void*))(base + offset_Transform_pos);
        
        void* pmUpdate = (void*)(base + offset_PM_Update);
        int res1 = DobbyHook(pmUpdate, (void*)hook_PM_Update, (void**)&orig_PM_Update);
        sprintf(buf, "Hook PM_Update res: %d", res1);
        WriteLog(buf);
    }
    
    void* egl_handle = dlopen("libEGL.so", RTLD_LAZY);
    if (egl_handle) {
        void* getProcAddrFunc = dlsym(egl_handle, "eglGetProcAddress");
        if (getProcAddrFunc) {
            int res2 = DobbyHook(getProcAddrFunc, (void*)hook_eglGetProcAddress, (void**)&orig_eglGetProcAddress);
            sprintf(buf, "Hook eglGetProcAddress res: %d", res2);
            WriteLog(buf);
        }
        
        void* swapFunc = dlsym(egl_handle, "eglSwapBuffers");
        if (swapFunc) {
            int res3 = DobbyHook(swapFunc, (void*)hook_eglSwapBuffers, (void**)&orig_eglSwapBuffers);
            sprintf(buf, "Hook eglSwapBuffers res: %d", res3);
            WriteLog(buf);
        }
    }
    return nullptr;
}

__attribute__((constructor))
void lib_main() {
    pthread_t pt; pthread_create(&pt, nullptr, hack_thread, nullptr); pthread_detach(pt);
}
