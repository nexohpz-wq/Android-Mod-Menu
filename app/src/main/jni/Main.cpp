#include <jni.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES3/gl3.h>
#include <dlfcn.h>
#include <pthread.h>
#include <unistd.h>
#include <cmath>
#include <android/log.h>
#include "Includes/Utils.hpp"
#include "dobby.h"

#define LOG_TAG "ESP_TEST"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

GLuint shaderProgram = 0;
GLint positionLoc = -1;
GLuint vao = 0;
bool shaderInitialized = false;

// Cargar funciones de VAO (OpenGL ES 3.0)
void (*glGenVertexArraysOES)(GLsizei, GLuint*) = nullptr;
void (*glBindVertexArrayOES)(GLuint) = nullptr;

typedef EGLBoolean (*eglSwapBuffers_t)(EGLDisplay dpy, EGLSurface surf);
eglSwapBuffers_t orig_eglSwapBuffers = nullptr;

GLuint CreateShaderProgram() {
    const char* vSource = "attribute vec4 vPosition; void main() { gl_Position = vPosition; }";
    const char* fSource = "precision mediump float; void main() { gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); }"; 
    
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vSource, NULL); glCompileShader(vShader);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fSource, NULL); glCompileShader(fShader);
    GLuint program = glCreateProgram();
    glAttachShader(program, vShader); glAttachShader(program, fShader);
    glLinkProgram(program);
    return program;
}

EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surf) {
    if (!shaderInitialized) {
        shaderProgram = CreateShaderProgram();
        positionLoc = glGetAttribLocation(shaderProgram, "vPosition");
        
        // Obtener funciones para VAO
        glGenVertexArraysOES = (void(*)(GLsizei, GLuint*))eglGetProcAddress("glGenVertexArraysOES");
        glBindVertexArrayOES = (void(*)(GLuint))eglGetProcAddress("glBindVertexArrayOES");
        
        if (glGenVertexArraysOES) {
            glGenVertexArraysOES(1, &vao);
        }
        
        shaderInitialized = true;
        LOGI("Shader y VAO inicializados.");
    }

    if (shaderInitialized) {
        // 1. Forzar a dibujar en la pantalla principal (no en texturas de Unity)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        // 2. Desactivar recortes y profundidad de Unity
        glDisable(GL_SCISSOR_TEST);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        
        // 3. Activar el programa y el VAO
        glUseProgram(shaderProgram);
        if (glBindVertexArrayOES) glBindVertexArrayOES(vao);

        // 4. Dibujar el cuadrado rojo estático (Prueba)
        // Coordenadas NDC: Esquina superior izquierda
        GLfloat vertices[] = {
            -1.0f,  1.0f,   -0.5f,  1.0f,   // Línea superior
            -1.0f,  1.0f,   -1.0f,  0.5f,   // Línea izquierda
            -0.5f,  1.0f,   -0.5f,  0.5f,   // Línea derecha
            -1.0f,  0.5f,   -0.5f,  0.5f    // Línea inferior
        };

        glVertexAttribPointer(positionLoc, 2, GL_FLOAT, GL_FALSE, 0, vertices);
        glEnableVertexAttribArray(positionLoc);
        glDrawArrays(GL_LINES, 0, 8);
        glDisableVertexAttribArray(positionLoc);

        // Restaurar VAO
        if (glBindVertexArrayOES) glBindVertexArrayOES(0);
        
        // Log para saber que está dibujando (solo imprimirá una vez por segundo aprox)
        static int frameCount = 0;
        if (frameCount++ % 60 == 0) LOGI("Dibujando cuadrado rojo...");
    }

    return orig_eglSwapBuffers(dpy, surf);
}

void* hack_thread(void*) {
    do { sleep(1); } while (!isLibraryLoaded("libil2cpp.so"));
    LOGI("Iniciando hook...");

    void* egl_handle = dlopen("libEGL.so", RTLD_LAZY);
    if (egl_handle) {
        void* swapFunc = dlsym(egl_handle, "eglSwapBuffers");
        if (swapFunc) {
            DobbyHook(swapFunc, (void*)hook_eglSwapBuffers, (void**)&orig_eglSwapBuffers);
            LOGI("Hook en eglSwapBuffers EXITOSO.");
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
