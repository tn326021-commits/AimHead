#pragma once
#include <cstdint>
#include <cstring>
#include <android/log.h>

#define LOG_TAG "AimPatch"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// ----------------------------------------------------------------
// IL2CPP Runtime type stubs (Unity 2020+ ARM64)
// ----------------------------------------------------------------

struct Il2CppObject {
    void* klass;
    void* monitor;
};

// UnityEngine.Transform wrapper
struct Transform : Il2CppObject {
    // position, rotation, etc accessed via method pointers
};

// Vector3 value type
struct Vector3 {
    float x, y, z;
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
};

// Unity Transform native method signatures (ARM64 ABI)
typedef Vector3 (*fn_Transform_get_position)(Transform* self);
typedef void    (*fn_Transform_set_position)(Transform* self, Vector3 pos);
typedef Vector3 (*fn_Transform_get_eulerAngles)(Transform* self);

// Get runtime base of libil2cpp.so (or the game's main lib)
inline uintptr_t GetBase(const char* lib = "libil2cpp.so") {
    char line[512];
    uintptr_t base = 0;
    FILE* fp = fopen("/proc/self/maps", "r");
    if (!fp) return 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, lib)) {
            base = (uintptr_t)strtoul(line, nullptr, 16);
            break;
        }
    }
    fclose(fp);
    return base;
}

// Read typed value from an object at given field offset
template<typename T>
inline T ReadField(void* obj, uintptr_t offset) {
    T val{};
    if (!obj) return val;
    memcpy(&val, reinterpret_cast<uint8_t*>(obj) + offset, sizeof(T));
    return val;
}

// Write typed value to an object at given field offset
template<typename T>
inline void WriteField(void* obj, uintptr_t offset, T val) {
    if (!obj) return;
    memcpy(reinterpret_cast<uint8_t*>(obj) + offset, &val, sizeof(T));
}

// Call a method by absolute address (player as first arg — "this")
template<typename Ret, typename... Args>
inline Ret CallMethod(uintptr_t addr, Args... args) {
    using Fn = Ret(*)(Args...);
    return reinterpret_cast<Fn>(addr)(args...);
}
