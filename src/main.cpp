#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "HitboxPatch", __VA_ARGS__)

struct Vector3 { float x, y, z; };

struct PatchTarget {
    uintptr_t offset;
    Vector3 value;
};

static uintptr_t lib_base = 0;

PatchTarget head_hitbox_patch = { 
    0x884b850, 
    { 3.0f, 3.0f, 3.0f } 
};

void ApplyPatch(uintptr_t absolute_addr, Vector3 val) {
    uintptr_t page_size = sysconf(_SC_PAGESIZE);
    uintptr_t page_start = absolute_addr & ~(page_size - 1);
    
    if (mprotect((void*)page_start, page_size * 2, PROT_READ | PROT_WRITE | PROT_EXEC) != 0) {
        LOGI("[!] mprotect failed at %p", (void*)absolute_addr);
        return;
    }

    Vector3* ptr = (Vector3*)absolute_addr;
    ptr->x = val.x;
    ptr->y = val.y;
    ptr->z = val.z;

    LOGI("[+] Head Hitbox x3 Patched at %p", (void*)absolute_addr);
    mprotect((void*)page_start, page_size * 2, PROT_READ | PROT_EXEC);
}

void FindBaseAddress() {
    FILE* maps = fopen("/proc/self/maps", "r");
    if (!maps) return;
    
    char line[512];
    while (fgets(line, sizeof(line), maps)) {
        if (strstr(line, "libil2cpp.so") && strstr(line, "r-xp")) {
            uintptr_t start;
            if (sscanf(line, "%lx-", &start) == 1) {
                lib_base = start;
                LOGI("[Base] Found libil2cpp.so at %p", (void*)lib_base);
                break;
            }
        }
    }
    fclose(maps);
}

void ExecutePatches() {
    if (lib_base == 0) {
        LOGI("[!] libil2cpp.so not loaded yet.");
        return;
    }
    uintptr_t abs_addr = lib_base + head_hitbox_patch.offset;
    ApplyPatch(abs_addr, head_hitbox_patch.value);
}

typedef void* (*dlopen_t)(const char*, int);
static dlopen_t orig_dlopen = nullptr;

void* hook_dlopen(const char* filename, int flag) {
    void* handle = orig_dlopen(filename, flag);
    if (filename && strstr(filename, "libil2cpp.so")) {
        LOGI("[Auto-Loader] libil2cpp.so detected.");
        usleep(1000000); 
        FindBaseAddress();
        ExecutePatches();
    }
    return handle;
}

void inline_hook(void* target, void* replace, void** backup) {
    #if defined(__aarch64__)
    uintptr_t page_size = sysconf(_SC_PAGESIZE);
    uintptr_t page_start = (uintptr_t)target & ~(page_size - 1);
    mprotect((void*)page_start, page_size * 2, PROT_READ | PROT_WRITE | PROT_EXEC);
    memcpy(backup, target, 4);
    uint32_t* code = (uint32_t*)target;
    code[0] = 0x58000050;
    code[1] = 0xd61f0200;
    *(void**)((uintptr_t)target + 8) = replace;
    __builtin___clear_cache((char*)target, (char*)target + 16);
    mprotect((void*)page_start, page_size * 2, PROT_READ | PROT_EXEC);
    #endif
}

__attribute__((constructor))
static void on_load() {
    LOGI("========================================");
    LOGI(" Head Hitbox x3 Patch Loaded            ");
    LOGI("========================================");
    void* dlopen_addr = dlsym(RTLD_DEFAULT, "dlopen");
    if (dlopen_addr) {
        void* backup_code = mmap(nullptr, 16, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        inline_hook(dlopen_addr, (void*)hook_dlopen, (void**)backup_code);
        orig_dlopen = (dlopen_t)backup_code;
    }
}
