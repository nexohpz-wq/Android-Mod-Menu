#include "obfuscate.h"
#include "Utils.hpp"
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <cstdio>

std::map<std::string, uintptr_t> lib_links;
bool mainLibLoaded = false;

uintptr_t getLibraryAddress(const char *libraryName) {
    if (!lib_links.count(libraryName)) {
        xdl_info_t info; void *handle = xdl_open(libraryName, XDL_DEFAULT);
        memset(&info, 0, sizeof(xdl_info_t));
        if (0 > xdl_info(handle, XDL_DI_DLINFO, &info)) {
            xdl_close(handle); return 0;
        } else { lib_links[libraryName] = (uintptr_t) info.dli_fbase; xdl_close(handle); }
    }
    return lib_links[libraryName];
}

void* getSymAddress(const char *libraryName, const char *SymName, bool relative) {
    xdl_info_t info; void *handle = xdl_open(libraryName, XDL_DEFAULT);
    if (handle == nullptr) { return nullptr; }
    memset(&info, 0, sizeof(xdl_info_t));
    if (0 > xdl_info(handle, XDL_DI_DLINFO, &info)) {}
    void *symbol_addr = xdl_sym(handle, SymName, nullptr);
    if (symbol_addr == nullptr) { symbol_addr = xdl_dsym(handle, SymName, nullptr); }
    xdl_close(handle);
    if (relative) { return (void*)((uintptr_t) symbol_addr - (uintptr_t) info.dli_fbase); } else return symbol_addr;
}

void* getAbsAddress(const char *libraryName, uintptr_t relativeAddr) {
    if (!lib_links.count(libraryName)) { lib_links[libraryName] = getLibraryAddress(libraryName); }
    if (!lib_links.count(libraryName)) return nullptr;
    return (void*)(lib_links[libraryName] + relativeAddr);
}

std::string getRelativeAddress(const char *libraryName, const char *rootOffset, const char *addOffset) {
    uintptr_t offset = str2offset(rootOffset); uintptr_t offset2 = str2offset(addOffset); uintptr_t result;
    if (offset != 0) { result = offset + offset2; } else { result = (uintptr_t)getSymAddress(libraryName, rootOffset, true) + offset2; }
    std::stringstream ss; ss << "0x" << std::hex << std::uppercase << result; return ss.str();
}

void* getAbsoluteAddress(const char *libraryName, const char *relative) {
    uintptr_t offset = str2offset(relative);
    if(offset != 0) { return getAbsAddress(libraryName, offset); } else { return getSymAddress(libraryName, relative, false); }
}

jboolean isGameLibLoaded(JNIEnv *env, jobject thiz) { return mainLibLoaded; }

bool isLibraryLoaded(const char *libraryName) {
    char line[512] = {0}; FILE *fp = fopen("/proc/self/maps", "rt");
    if (fp != nullptr) {
        while (fgets(line, sizeof(line), fp)) {
            std::string a = line;
            if (strstr(line, libraryName)) { mainLibLoaded = true; return true; }
        }
        fclose(fp);
    }
    return false;
}

uintptr_t str2offset(const char *c) {
    int base = 16;
    if (sizeof(uintptr_t) == sizeof(unsigned long)) { return strtoul(c, nullptr, base); }
    return strtoull(c, nullptr, base);
}
