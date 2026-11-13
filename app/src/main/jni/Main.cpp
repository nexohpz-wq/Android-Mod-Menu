#include <jni.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES3/gl3.h>
#include <dlfcn.h>
#include <pthread.h>
#include <unistd.h>
#include <cmath>
#include <set>
#include <algorithm>
#include <android/log.h>
#include "Includes/Utils.hpp"
#include "dobby.h"

#define LOG_TAG "ESP_REAL"

struct Vector3 { float x, y, z; };

// TUS OFFSETS EXACTOS
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

// OpenGL
GLuint shaderProgram = 0;
GLint positionLoc = -1;
GLuint vao = 0;
bool shaderInitialized = false;
void (*glGenVertexArraysOES)(GLsizei, GLuint*) = nullptr;
void (*glBindVertexArrayOES)(GLuint) = nullptr;

void hook_PM_Update(void* instance) {
    if (instance) active_players.insert(instance);
    if (orig_PM_Update) orig_PM_Update(instance);
}

typedef EGLBoolean (*eglSwapBuffers_t)(EGLDisplay dpy, EGLSurface surf);
eglSwapBuffers_t orig_eglSwapBuffers = nullptr;

GLuint CreateShaderProgram() {
    const char* vSource = "attribute vec4 vPosition; void main() { gl_Position = vPosition; }";
    const char* fSource = "precision mediump float; void main() { gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); }"; // Rojo
    
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vSource, NULL); glCompileShader(vShader);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fSource, NULL); glCompileShader(fShader);
    GLuint program = glCreateProgram();
    glAttachShader(program, vShader); glAttachShader(program, fShader);
    glLinkProgram(program);
    return program;
}

void DrawBox(float sx, float sy, float w, float h, float sw, float sh) {
    auto toNDC = [](float x, float y, float scrW, float scrH) {
        return std::make_pair((x / scrW) * 2.0f - 1.0f, 1.0f - (y / scrH) * 2.0f);
    };

    auto p1 = toNDC(sx, sy, sw, sh);
    auto p2 = toNDC(sx + w, sy, sw, sh);
    auto p3 = toNDC(sx, sy + h, sw, sh);
    auto p4 = toNDC(sx + w, sy + h, sw, sh);

    GLfloat vertices[] = {
        p1.first, p1.second, p2.first, p2.second,
        p1.first, p1.second, p3.first, p3.second,
        p2.first, p2.second, p4.first, p4.second,
        p3.first, p3.second, p4.first, p4.second
    };

    glVertexAttribPointer(positionLoc, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(positionLoc);
    glDrawArrays(GL_LINES, 0, 8);
    glDisableVertexAttribArray(positionLoc);
}

EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surf) {
    if (!shaderInitialized) {
        shaderProgram = CreateShaderProgram();
        positionLoc = glGetAttribLocation(shaderProgram, "vPosition");
        
        glGenVertexArraysOES = (void(*)(GLsizei, GLuint*))eglGetProcAddress("glGenVertexArraysOES");
        glBindVertexArrayOES = (void(*)(GLuint))eglGetProcAddress("glBindVertexArrayOES");
        if (glGenVertexArraysOES) glGenVertexArraysOES(1, &vao);
        
        shaderInitialized = true;
    }

    if (shaderInitialized && Camera_get_main && Camera_WorldToScreenPoint && Component_get_transform && Transform_get_position) {
        void* cam = Camera_get_main();
        if (cam) {
            GLint viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);
            float sw = viewport[2];
            float sh = viewport[3];

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDisable(GL_SCISSOR_TEST);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_STENCIL_TEST);
            glUseProgram(shaderProgram);
            if (glBindVertexArrayOES) glBindVertexArrayOES(vao);

            for (void* player : active_players) {
                void* trans = Component_get_transform(player);
                if (!trans) continue;

                Vector3 pos = Transform_get_position(trans);
                Vector3 headPos = pos;
                headPos.y += 1.7f;

                Vector3 screenHead = Camera_WorldToScreenPoint(cam, headPos, 0);
                Vector3 screenFeet = Camera_WorldToScreenPoint(cam, pos, 0);

                if (screenHead.z > 0 && screenFeet.z > 0) { 
                    float topY = std::min(screenHead.y, screenFeet.y);
                    float bottomY = std::max(screenHead.y, screenFeet.y);
                    float boxHeight = bottomY - topY;
                    
                    float boxWidth = boxHeight * 0.45f; 
                    
                    float x = screenHead.x - (boxWidth / 2);
                    float sy = sh - bottomY;

                    DrawBox(x, sy, boxWidth, boxHeight, sw, sh);
                }
            }
        }
    }
    return orig_eglSwapBuffers(dpy, surf);
}

void* hack_thread(void*) {
    do { sleep(1); } while (!isLibraryLoaded("libil2cpp.so"));

    // La suma manual de la base + el offset (Método infalible)
    uintptr_t base = getLibraryAddress("libil2cpp.so");
    
    Camera_get_main = (void*(*)())(base + offset_Camera_main);
    Camera_WorldToScreenPoint = (Vector3(*)(void*, Vector3, int))(base + offset_W2S);
    Component_get_transform = (void*(*)(void*))(base + offset_Component_transform);
    Transform_get_position = (Vector3(*)(void*))(base + offset_Transform_pos);

    void* pmUpdate = (void*)(base + offset_PM_Update);
    
    if (pmUpdate) DobbyHook(pmUpdate, (void*)hook_PM_Update, (void**)&orig_PM_Update);

    void* egl_handle = dlopen("libEGL.so", RTLD_LAZY);
    if (egl_handle) {
        void* swapFunc = dlsym(egl_handle, "eglSwapBuffers");
        if (swapFunc) DobbyHook(swapFunc, (void*)hook_eglSwapBuffers, (void**)&orig_eglSwapBuffers);
    }
    return nullptr;
}

__attribute__((constructor))
void lib_main() {
    pthread_t pt;
    pthread_create(&pt, nullptr, hack_thread, nullptr);
    pthread_detach(pt);
}
