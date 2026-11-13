#include <jni.h>
#include <pthread.h>
#include <unistd.h>
#include "il2cpp_helpers.h"
#include "offsets.h"
#include "no_recoil.h"
#include "aim_neck.h"
#include "antiban.h"

// ----------------------------------------------------------------
// Entry point thread — waits for libil2cpp.so to load,
// then installs all hooks.
// ----------------------------------------------------------------

static void* patch_thread(void*) {
    LOGI("[AimPatch] Waiting for il2cpp...");

    // Wait until libil2cpp.so is mapped
    uintptr_t base = 0;
    for (int i = 0; i < 120 && base == 0; ++i) {
        base = GetBase("libil2cpp.so");
        if (!base) sleep(1);
    }

    if (!base) {
        LOGE("[AimPatch] libil2cpp.so not found after 120s — aborting");
        return nullptr;
    }

    LOGI("[AimPatch] libil2cpp.so base = 0x%lx", base);

    // 1. Install AntiBan FIRST (before any other hook)
    if (InstallAntiBan()) {
        LOGI("[AimPatch] AntiBan OK");
    } else {
        LOGE("[AimPatch] AntiBan partial");
    }

    // Small delay to let game finish loading DLLs
    sleep(2);

    // 2. Install No Recoil
    if (InstallNoRecoil(base)) {
        LOGI("[AimPatch] NoRecoil OK");
    } else {
        LOGE("[AimPatch] NoRecoil failed");
    }

    // 3. Install AimNeck
    if (InstallAimNeck(base)) {
        LOGI("[AimPatch] AimNeck OK");
    } else {
        LOGE("[AimPatch] AimNeck failed");
    }

    LOGI("[AimPatch] All patches installed.");
    return nullptr;
}

// ---- JNI_OnLoad — called when .so is dlopen'd ----
extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    pthread_t tid;
    pthread_create(&tid, nullptr, patch_thread, nullptr);
    pthread_detach(tid);
    return JNI_VERSION_1_6;
}
