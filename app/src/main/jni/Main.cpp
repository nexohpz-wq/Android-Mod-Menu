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
#include <string>
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
eglSwapBuffersWithDamage_t orig_eglSwapBuffersWithDamageEXT = nullptr;

typedef void* (*eglGetProcAddress_t)(const char *procname);
eglGetProcAddress_t orig_eglGetProcAddress = nullptr;

GLuint CreateShaderProgram() {
    const char* vSource = "attribute vec4 vPosition; void main() { gl_Position = vPosition; }";
    const char* fSource = "precision mediump float; void main() { gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); }";
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER); glShaderSource(vShader, 1, &vSource, NULL); glCompileShader(vShader);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER); glShaderSource(fShader, 1, &fSource, NULL); glCompileShader(fShader);
    GLuint program = glCreateProgram(); glAttachShader(program, vShader); glAttachShader(program, fShader); glLinkProgram(program);
    return program;
}

void DrawBox(float sx, float sy, float w, float h, float sw, float sh) {
    auto toNDC = [](float x, float y, float scrW, float scrH) {
        return std::make_pair((x / scrW) * 2.0f - 1.0f, 1.0f - (y / scrH) * 2.0f);
    };
    auto p1 = toNDC(sx, sy, sw, sh); auto p2 = toNDC(sx + w, sy, sw, sh);
    auto p3 = toNDC(sx, sy + h, sw, sh); auto p4 = toNDC(sx + w, sy + h, sw, sh);
    GLfloat vertices[] = { p1.first, p1.second, p2.first, p2.second, p1.first, p1.second, p3.first, p3.second, p2.first, p2.second, p4.first, p4.second, p3.first, p3.second, p4.first, p4.second };
    glVertexAttribPointer(positionLoc, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(positionLoc);
    glDrawArrays(GL_LINES, 0, 8);
    glDisableVertexAttribArray(positionLoc);
}

void DrawESP() {
    static int callCount = 0;
    if (callCount++ % 120 == 0) {
        char buf[128];
        sprintf(buf, "DrawESP llamado. Jugadores: %zu", active_players.size());
        WriteLog(buf);
    }

    if (!shaderInitialized) {
        shaderProgram = CreateShaderProgram();
        positionLoc = glGetAttribLocation(shaderProgram, "vPosition");
        glGenVertexArraysOES = (void(*)(GLsizei, GLuint*))eglGetProcAddress("glGenVertexArraysOES");
        glBindVertexArrayOES = (void(*)(GLuint))eglGetProcAddress("glBindVertexArrayOES");
        if (glGenVertexArraysOES) glGenVertexArraysOES(1, &vao);
        shaderInitialized = true;
        WriteLog("Shader inicializado.");
    }

    if (shaderInitialized && Camera_get_main && Camera_WorldToScreenPoint) {
        void* cam = Camera_get_main();
        if (!cam) {
            if (callCount % 120 == 0) WriteLog("Cámara NULL.");
            return;
        }
        
        GLint viewport[4]; glGetIntegerv(GL_VIEWPORT, viewport);
        float sw = viewport[2]; float sh = viewport[3];

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_SCISSOR_TEST); glDisable(GL_DEPTH_TEST); glDisable(GL_STENCIL_TEST);
        glUseProgram(shaderProgram);
        if (glBindVertexArrayOES) glBindVertexArrayOES(vao);

        std::lock_guard<std::mutex> lock(players_mutex);
        int drawn = 0;
        for (void* player : active_players) {
            void* trans = Component_get_transform(player);
            if (!trans) continue;
            Vector3 pos = Transform_get_position(trans);
            Vector3 headPos = pos; headPos.y += 1.75f;
            Vector3 screenHead = Camera_WorldToScreenPoint(cam, headPos, 0);
            Vector3 screenFeet = Camera_WorldToScreenPoint(cam, pos, 0);

            if (screenHead.z > 0 && screenFeet.z > 0) { 
                float topY = std::max(screenHead.y, screenFeet.y);
                float bottomY = std::min(screenHead.y, screenFeet.y);
                float boxHeight = topY - bottomY;
                float boxWidth = boxHeight * 0.45f; 
                float x = screenHead.x - (boxWidth / 2);
                float sy = sh - topY;
                DrawBox(x, sy, boxWidth, boxHeight, sw, sh);
                drawn++;
            }
        }
        if (drawn > 0 && callCount % 120 == 0) {
            WriteLog("Cajas dibujadas en pantalla!");
        }
    }
}

EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surf) {
    static int count = 0;
    if (count++ % 60 == 0) WriteLog("hook_eglSwapBuffers ejecutado.");
    DrawESP();
    return orig_eglSwapBuffers(dpy, surf);
}

EGLBoolean hook_eglSwapBuffersWithDamageKHR(EGLDisplay dpy, EGLSurface surf, EGLint* rects, EGLint n_rects) {
    static int count = 0;
    if (count++ % 60 == 0) WriteLog("hook_eglSwapBuffersWithDamageKHR ejecutado.");
    DrawESP();
    if (orig_eglSwapBuffersWithDamageKHR) return orig_eglSwapBuffersWithDamageKHR(dpy, surf, rects, n_rects);
    return EGL_TRUE;
}

EGLBoolean hook_eglSwapBuffersWithDamageEXT(EGLDisplay dpy, EGLSurface surf, EGLint* rects, EGLint n_rects) {
    static int count = 0;
    if (count++ % 60 == 0) WriteLog("hook_eglSwapBuffersWithDamageEXT ejecutado.");
    DrawESP();
    if (orig_eglSwapBuffersWithDamageEXT) return orig_eglSwapBuffersWithDamageEXT(dpy, surf, rects, n_rects);
    return EGL_TRUE;
}

void* hook_eglGetProcAddress(const char *procname) {
    if (procname) {
        if (strcmp(procname, "eglSwapBuffers") == 0) {
            if (!orig_eglSwapBuffers) orig_eglSwapBuffers = (eglSwapBuffers_t)orig_eglGetProcAddress(procname);
            return (void*)hook_eglSwapBuffers;
        }
        if (strcmp(procname, "eglSwapBuffersWithDamageKHR") == 0) {
            if (!orig_eglSwapBuffersWithDamageKHR) orig_eglSwapBuffersWithDamageKHR = (eglSwapBuffersWithDamage_t)orig_eglGetProcAddress(procname);
            return (void*)hook_eglSwapBuffersWithDamageKHR;
        }
        if (strcmp(procname, "eglSwapBuffersWithDamageEXT") == 0) {
            if (!orig_eglSwapBuffersWithDamageEXT) orig_eglSwapBuffersWithDamageEXT = (eglSwapBuffersWithDamage_t)orig_eglGetProcAddress(procname);
            return (void*)hook_eglSwapBuffersWithDamageEXT;
        }
    }
    return orig_eglGetProcAddress(procname);
}

void* hack_thread(void*) {
    WriteLog("hack_thread iniciado (esperando libil2cpp)");
    
    uintptr_t base = 0;
    while (base == 0) {
        base = getLibraryAddress("libil2cpp.so");
        if (base == 0) sleep(1);
    }
    WriteLog("libil2cpp.so cargado.");
    
    Camera_get_main = (void*(*)())(base + offset_Camera_main);
    Camera_WorldToScreenPoint = (Vector3(*)(void*, Vector3, int))(base + offset_W2S);
    Component_get_transform = (void*(*)(void*))(base + offset_Component_transform);
    Transform_get_position = (Vector3(*)(void*))(base + offset_Transform_pos);
    
    void* pmUpdate = (void*)(base + offset_PM_Update);
    DobbyHook(pmUpdate, (void*)hook_PM_Update, (void**)&orig_PM_Update);
    
    void* egl_handle = dlopen("libEGL.so", RTLD_LAZY);
    if (egl_handle) {
        void* getProcAddrFunc = dlsym(egl_handle, "eglGetProcAddress");
        if (getProcAddrFunc) {
            DobbyHook(getProcAddrFunc, (void*)hook_eglGetProcAddress, (void**)&orig_eglGetProcAddress);
            WriteLog("Hook eglGetProcAddress instalado.");
        }

        void* swapFunc = dlsym(egl_handle, "eglSwapBuffers");
        if (swapFunc) {
            DobbyHook(swapFunc, (void*)hook_eglSwapBuffers, (void**)&orig_eglSwapBuffers);
            WriteLog("Hook eglSwapBuffers instalado.");
        }
        
        void* swapKHR = dlsym(egl_handle, "eglSwapBuffersWithDamageKHR");
        if (swapKHR) {
            DobbyHook(swapKHR, (void*)hook_eglSwapBuffersWithDamageKHR, (void**)&orig_eglSwapBuffersWithDamageKHR);
            WriteLog("Hook eglSwapBuffersWithDamageKHR instalado.");
        }

        void* swapEXT = dlsym(egl_handle, "eglSwapBuffersWithDamageEXT");
        if (swapEXT) {
            DobbyHook(swapEXT, (void*)hook_eglSwapBuffersWithDamageEXT, (void**)&orig_eglSwapBuffersWithDamageEXT);
            WriteLog("Hook eglSwapBuffersWithDamageEXT instalado.");
        }
    }
    return nullptr;
}

extern "C" void StartESP() {
    pthread_t pt; 
    pthread_create(&pt, nullptr, hack_thread, nullptr); 
    pthread_detach(pt);
}

__attribute__((constructor))
void lib_main() {
    StartESP();
}
