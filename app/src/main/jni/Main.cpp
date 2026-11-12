#include <jni.h>
#include <pthread.h>
#include <unistd.h>
#include "Includes/Utils.hpp"
#include "Menu/Menu.h"

void* hack_thread(void*) {
    do { 
        sleep(1); 
    } while (!isLibraryLoaded("libil2cpp.so"));
    return nullptr;
}

__attribute__((constructor))
void lib_main() {
    pthread_t pt;
    pthread_create(&pt, nullptr, hack_thread, nullptr);
    pthread_detach(pt);
}
