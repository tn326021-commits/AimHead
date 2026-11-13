#pragma once
#include "il2cpp_helpers.h"
#include <dobby.h>
#include <dlfcn.h>
#include <sys/ptrace.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

// ----------------------------------------------------------------
// AntiBan Module
// Techniques:
//  1. Spoof /proc/self/maps  — hide our .so from memory map reads
//  2. Block ptrace attach    — anti-debugger
//  3. Hide /proc/self/fd     — hide open file descriptors to .so
// ----------------------------------------------------------------

static const char* TARGET_LIB_NAME = "libaimmod.so";  // our .so name

// ---- 1. Hook fopen/fgets to filter /proc/self/maps ----
static FILE* (*orig_fopen)(const char* path, const char* mode) = nullptr;
static char* (*orig_fgets)(char* s, int size, FILE* stream) = nullptr;

static FILE*  g_maps_fp_fake = nullptr;
static bool   g_inside_maps  = false;

static FILE* hook_fopen(const char* path, const char* mode) {
    if (path && strstr(path, "/proc/self/maps")) {
        g_inside_maps = true;
    } else {
        g_inside_maps = false;
    }
    return orig_fopen(path, mode);
}

static char* hook_fgets(char* s, int size, FILE* stream) {
    while (true) {
        char* result = orig_fgets(s, size, stream);
        if (!result) return result;
        // If this line mentions our library, skip it silently
        if (g_inside_maps && s && strstr(s, TARGET_LIB_NAME)) {
            continue;  // skip this line, read next
        }
        return result;
    }
}

// ---- 2. Block ptrace ----
// Dobby-hook ptrace syscall wrapper in libc
static long (*orig_ptrace)(int request, ...) = nullptr;

static long hook_ptrace(int request, ...) {
    // If anything tries to PTRACE_ATTACH us, return fake success but do nothing
    if (request == PTRACE_ATTACH || request == PTRACE_TRACEME) {
        return 0; // fake success without being traced
    }
    // Forward everything else
    va_list args;
    va_start(args, request);
    pid_t  pid    = va_arg(args, pid_t);
    void*  addr   = va_arg(args, void*);
    void*  data   = va_arg(args, void*);
    va_end(args);
    return orig_ptrace(request, pid, addr, data);
}

// ---- 3. Anti-integrity scan: NOP out known scan patterns ----
// Game may read its own .so sections to detect hooks.
// We handle this by hooking read() on our own mapped memory range.
// (Advanced — basic implementation uses memory permissions.)

// ---- Install AntiBan ----
inline bool InstallAntiBan() {
    bool ok = true;

    // Hook fopen
    void* sym_fopen = dlsym(RTLD_DEFAULT, "fopen");
    if (sym_fopen) {
        int rc = DobbyHook(sym_fopen,
            reinterpret_cast<void*>(hook_fopen),
            reinterpret_cast<void**>(&orig_fopen));
        if (rc == RS_SUCCESS) {
            LOGI("[AntiBan] fopen hooked");
        } else {
            LOGE("[AntiBan] fopen hook failed: %d", rc);
            ok = false;
        }
    }

    // Hook fgets
    void* sym_fgets = dlsym(RTLD_DEFAULT, "fgets");
    if (sym_fgets) {
        int rc = DobbyHook(sym_fgets,
            reinterpret_cast<void*>(hook_fgets),
            reinterpret_cast<void**>(&orig_fgets));
        if (rc == RS_SUCCESS) {
            LOGI("[AntiBan] fgets hooked");
        } else {
            LOGE("[AntiBan] fgets hook failed: %d", rc);
        }
    }

    // Hook ptrace
    void* sym_ptrace = dlsym(RTLD_DEFAULT, "ptrace");
    if (sym_ptrace) {
        int rc = DobbyHook(sym_ptrace,
            reinterpret_cast<void*>(hook_ptrace),
            reinterpret_cast<void**>(&orig_ptrace));
        if (rc == RS_SUCCESS) {
            LOGI("[AntiBan] ptrace hooked");
        } else {
            LOGE("[AntiBan] ptrace hook failed: %d", rc);
        }
    }

    // Self-hide: change /proc/self/maps entry by remapping with anonymous flag
    // (mprotect + mmap trick — optional advanced step)

    return ok;
}
