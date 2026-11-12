#include <jni.h>
#include "Includes/Includes.h"
#include "Includes/Utils.hpp"
#include "Menu/Menu.h"
#include <thread>
#include <chrono>
#include <android/log.h>

#define LOG_TAG "ESP_MOD"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

bool skeleton_esp = true;

void* esp_thread(void*) {
    while (true) {
        if (skeleton_esp) {
            ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
            ImVec2 screen = ImGui::GetIO().DisplaySize;
            draw_list->AddLine(ImVec2(screen.x / 2 - 50, screen.y / 2), ImVec2(screen.x / 2 + 50, screen.y / 2), IM_COL32(255, 0, 0, 255), 3.0f);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); 
    }
    return nullptr;
}

void* hack_thread(void*) {
    do { sleep(1); } while (!isLibraryLoaded("libil2cpp.so"));
    LOGI("Mod Menu y ESP Iniciados");
    pthread_t pt;
    pthread_create(&pt, nullptr, esp_thread, nullptr);
    return nullptr;
}

void DrawMenu() {
    if (ImGui::BeginTabBar("Features")) {
        if (ImGui::BeginTabItem("ESP Hack")) {
            ImGui::Checkbox("Skeleton ESP (Test)", &skeleton_esp);
            ImGui::Text("Linea roja = Dibujo Nativo OK");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

__attribute__((constructor))
void lib_main() {
    pthread_t pt;
    pthread_create(&pt, nullptr, hack_thread, nullptr);
}
