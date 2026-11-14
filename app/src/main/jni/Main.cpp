#include <jni.h>
#include <pthread.h>
#include <unistd.h>
#include <dlfcn.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <set>
#include <mutex>
#include "Includes/Utils.hpp"
#include "Dobby/dobby.h"
#include "Menu/ImGui/imgui.h"
#include "Menu/ImGui/imgui_internal.h"
#include "Menu/ImGui/backends/imgui_impl_opengl3.h"

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

bool imgui_initialized = false;
int screen_h = 0;

void hook_PM_Update(void* instance) {
    if (instance) {
        std::lock_guard<std::mutex> lock(players_mutex);
        if (active_players.size() < 100) active_players.insert(instance);
    }
    if (orig_PM_Update) orig_PM_Update(instance);
}

typedef EGLBoolean (*eglSwapBuffers_t)(EGLDisplay dpy, EGLSurface surf);
eglSwapBuffers_t orig_eglSwapBuffers = nullptr;

EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surf) {
    if (!imgui_initialized) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = NULL;
        ImGui_ImplOpenGL3_Init("#version 300 es");
        imgui_initialized = true;
    }

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    screen_h = viewport[3];

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    if (Camera_get_main && Camera_WorldToScreenPoint) {
        void* cam = Camera_get_main();
        if (cam) {
            ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
            std::lock_guard<std::mutex> lock(players_mutex);
            
            for (void* player : active_players) {
                void* trans = Component_get_transform(player);
                if (!trans) continue;
                
                Vector3 pos = Transform_get_position(trans);
                Vector3 headPos = pos; headPos.y += 1.75f;
                
                Vector3 sHead = Camera_WorldToScreenPoint(cam, headPos, 0);
                Vector3 sFeet = Camera_WorldToScreenPoint(cam, pos, 0);

                if (sHead.z > 0 && sFeet.z > 0) { 
                    float topY = std::max(sHead.y, sFeet.y);
                    float bottomY = std::min(sHead.y, sFeet.y);
                    float h = topY - bottomY;
                    float w = h * 0.45f; 
                    float x = sHead.x - (w / 2);
                    float y = screen_h - topY;

                    draw_list->AddRect(
                        ImVec2(x, y), 
                        ImVec2(x + w, y + h), 
                        IM_COL32(255, 0, 0, 255), 
                        0.0f, 0, 2.0f 
                    );
                }
            }
        }
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return orig_eglSwapBuffers(dpy, surf);
}

void* hack_thread(void*) {
    do { sleep(1); } while (!isLibraryLoaded("libil2cpp.so"));
    uintptr_t base = getLibraryAddress("libil2cpp.so");
    
    if (base != 0) {
        Camera_get_main = (void*(*)())(base + offset_Camera_main);
        Camera_WorldToScreenPoint = (Vector3(*)(void*, Vector3, int))(base + offset_W2S);
        Component_get_transform = (void*(*)(void*))(base + offset_Component_transform);
        Transform_get_position = (Vector3(*)(void*))(base + offset_Transform_pos);
        
        void* pmUpdate = (void*)(base + offset_PM_Update);
        DobbyHook(pmUpdate, (void*)hook_PM_Update, (void**)&orig_PM_Update);
    }
    
    void* egl_handle = dlopen("libEGL.so", RTLD_LAZY);
    if (egl_handle) {
        void* swapFunc = dlsym(egl_handle, "eglSwapBuffers");
        if (swapFunc) DobbyHook(swapFunc, (void*)hook_eglSwapBuffers, (void**)&orig_eglSwapBuffers);
    }
    return nullptr;
}

__attribute__((constructor))
void lib_main() {
    pthread_t pt; pthread_create(&pt, nullptr, hack_thread, nullptr); pthread_detach(pt);
}
