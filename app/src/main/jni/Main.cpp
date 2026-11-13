// ... (todo el código de arriba se queda igual)

__attribute__((constructor))
void lib_main() {
    // Escribir un archivo de prueba para saber si Zygisk cargó esto
    FILE *f = fopen("/data/data/com.ScarletWork.CounterFlame/files/esp_started.txt", "w");
    if (f) {
        fprintf(f, "lib_main ejecutado! El ESP está en memoria.\n");
        fclose(f);
    }

    pthread_t pt;
    pthread_create(&pt, nullptr, hack_thread, nullptr);
    pthread_detach(pt);
}
