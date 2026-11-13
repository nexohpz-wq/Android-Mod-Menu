#include <jni.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <dlfcn.h>
#include <pthread.h>
#include <unistd.h>
#include <cmath>
#include <set>
#include <algorithm> // Para std::min y std::max
#include <android/log.h>
#include "Includes/Utils.hpp"
#include "dobby.h"

#define LOG_TAG "ESP_NATIVE"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

struct Vector3 { float x, y, z; };

// Tus offsets exactos extraídos de la memoria
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

void hook_PM_Update(void* instance) {
    if (instance) active_players.insert(instance);
    if (orig_PM_Update) orig_PM_Update(instance);
}

typedef EGLBoolean (*eglSwapBuffers_t)(EGLDisplay dpy, EGLSurface surf);
eglSwapBuffers_t orig_eglSwapBuffers = nullptr;

void DrawBox(float x, float y, float w, float h) {
    GLfloat vertices[] = {
        x, y, x + w, y,          // Arriba
        x, y + h, x + w, y + h,  // Abajo
        x, y, x, y + h,          // Izquierda
        x + w, y, x + w, y + h   // Derecha
    };
    glLineWidth(2.0f);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_LINES, 0, 8);
}

EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surf) {
    if (Camera_get_main && Camera_WorldToScreenPoint && Component_get_transform && Transform_get_position) {
        void* cam = Camera_get_main();
        if (cam) {
            GLint viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);
            float screenHeight = viewport[3];

            for (void* player : active_players) {
                void* trans = Component_get_transform(player);
                if (!trans) continue;

                Vector3 pos = Transform_get_position(trans);
                
                // Ajuste de altura: 1.75f es la estatura promedio de los modelos
                Vector3 headPos = pos;
                headPos.y += 1.75f; 

                Vector3 screenHead = Camera_WorldToScreenPoint(cam, headPos, 0);
                Vector3 screenFeet = Camera_WorldToScreenPoint(cam, pos, 0);

                // Si está en pantalla y al frente de la cámara
                if (screenHead.z > 0 && screenFeet.z > 0) { 
                    
                    // Calcular altura exacta del modelo en la pantalla
                    float topY = std::min(screenHead.y, screenFeet.y);
                    float bottomY = std::max(screenHead.y, screenFeet.y);
                    float boxHeight = bottomY - topY;
                    
                    // Calcular ancho proporcional (0.45f hace que quede pegada a los hombros)
                    float boxWidth = boxHeight * 0.45f; 
                    
                    // Centrar la caja horizontalmente en el jugador
                    float x = screenHead.x - (boxWidth / 2);
                    
                    // Invertir la Y para OpenGL (que dibuja desde arriba hacia abajo)
                    float y = screenHeight - bottomY;

                    // Dibujar la caja
                    glDisable(GL_DEPTH_TEST); // Para que se vea a través de las paredes
                    DrawBox(x, y, boxWidth, boxHeight);
                    glEnable(GL_DEPTH_TEST);
                }
            }
        }
    }
    return orig_eglSwapBuffers(dpy, surf);
}

void* hack_thread(void*) {
    do { sleep(1); } while (!isLibraryLoaded("libil2cpp.so"));
    LOGI("Juego cargado. Iniciando ESP...");

    Camera_get_main = (void*(*)())getAbsoluteAddress("libil2cpp.so", offset_Camera_main);
    Camera_WorldToScreenPoint = (Vector3(*)(void*, Vector3, int))getAbsoluteAddress("libil2cpp.so", offset_W2S);
    Component_get_transform = (void*(*)(void*))getAbsoluteAddress("libil2cpp.so", offset_Component_transform);
    Transform_get_position = (Vector3(*)(void*))getAbsoluteAddress("libil2cpp.so", offset_Transform_pos);

    void* pmUpdate = getAbsoluteAddress("libil2cpp.so", offset_PM_Update);
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
