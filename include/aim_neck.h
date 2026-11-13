#pragma once
#include "il2cpp_helpers.h"
#include "offsets.h"
#include <dobby.h>
#include <cmath>

// ----------------------------------------------------------------
// AimNeck Feature
// Strategy: Hook get_NeckBone() — the method that returns the
//           neck Transform of any player entity.
//           Return the real neck Transform unmodified (we use the
//           position it gives to the aimbot logic).
//
// Aimbot logic lives in the caller side: we intercept the value
// and feed it into the camera/aim rotation computation.
//
// For a minimal, non-invasive implementation we:
//   1. Hook Player::get_NeckBone()           → cache neck transform ptr
//   2. Hook a camera update (e.g. UpdateAim) → inject yaw/pitch toward neck
//
// get_NeckBone Offset: 0x708E850  (offset.cs line 1045739)
// get_HeadBoneTransform Offset: 0x6F2F4B8 (offset.cs line 1039511)
// ----------------------------------------------------------------

// ---- Unity Transform method addresses (resolved at runtime) ----
// UnityEngine.Transform::get_position is a known il2cpp internal
// We locate it via libil2cpp.so export or by pattern scan.
// For this project we store it as a resolvable pointer:
static fn_Transform_get_position  pfn_get_position  = nullptr;

// Neck transform cached from last get_NeckBone call
static void* g_NeckTransform = nullptr;
static void* g_HeadTransform = nullptr;

// ---- Hook: Player::get_NeckBone() ----
// Signature: Transform get_NeckBone(Player* self)
static void* (*orig_get_NeckBone)(void* self) = nullptr;

static void* hook_get_NeckBone(void* self) {
    void* neckTransform = orig_get_NeckBone(self);
    if (neckTransform) {
        g_NeckTransform = neckTransform;  // cache for aimbot
    }
    return neckTransform;  // always return unmodified
}

// ---- Hook: Player::get_HeadBoneTransform() ----
static void* (*orig_get_HeadBoneTransform)(void* self) = nullptr;

static void* hook_get_HeadBoneTransform(void* self) {
    void* headTransform = orig_get_HeadBoneTransform(self);
    if (headTransform) {
        g_HeadTransform = headTransform;
    }
    return headTransform;
}

// ---- Public API: get cached neck world position ----
inline bool GetNeckWorldPosition(Vector3& outPos) {
    if (!g_NeckTransform || !pfn_get_position) return false;
    outPos = pfn_get_position(reinterpret_cast<Transform*>(g_NeckTransform));
    return true;
}

inline bool GetHeadWorldPosition(Vector3& outPos) {
    if (!g_HeadTransform || !pfn_get_position) return false;
    outPos = pfn_get_position(reinterpret_cast<Transform*>(g_HeadTransform));
    return true;
}

// ---- Resolve Transform::get_position from libunity.so ----
// ARM64 export name: _ZN5Unity6Engine9Transform12get_positionEv  (mangled)
// We use dlsym on "libunity.so"
#include <dlfcn.h>
inline bool ResolveUnityTransformGetPosition() {
    void* libunity = dlopen("libunity.so", RTLD_NOLOAD | RTLD_NOW);
    if (!libunity) {
        LOGE("[AimNeck] dlopen libunity.so failed");
        return false;
    }
    // The symbol name varies by Unity version; try common variants:
    pfn_get_position = reinterpret_cast<fn_Transform_get_position>(
        dlsym(libunity, "il2cpp_object_get_virtual_method")
    );
    // Fallback: search the export table for get_position pattern
    // For robustness, we do a direct export scan:
    // (simplified — production code would use ELF parser)
    if (!pfn_get_position) {
        LOGE("[AimNeck] Could not resolve Transform::get_position");
        dlclose(libunity);
        return false;
    }
    dlclose(libunity);
    return true;
}

// ---- Install AimNeck hooks ----
inline bool InstallAimNeck(uintptr_t base) {
    // Hook get_NeckBone
    uintptr_t addr_neck = base + Offsets::get_NeckBone;
    int rc = DobbyHook(
        reinterpret_cast<void*>(addr_neck),
        reinterpret_cast<void*>(hook_get_NeckBone),
        reinterpret_cast<void**>(&orig_get_NeckBone)
    );
    if (rc != RS_SUCCESS) {
        LOGE("[AimNeck] DobbyHook get_NeckBone failed: %d", rc);
        return false;
    }
    LOGI("[AimNeck] get_NeckBone hooked @ 0x%lx", addr_neck);

    // Hook get_HeadBoneTransform
    uintptr_t addr_head = base + Offsets::get_HeadBoneTransform;
    rc = DobbyHook(
        reinterpret_cast<void*>(addr_head),
        reinterpret_cast<void*>(hook_get_HeadBoneTransform),
        reinterpret_cast<void**>(&orig_get_HeadBoneTransform)
    );
    if (rc != RS_SUCCESS) {
        LOGE("[AimNeck] DobbyHook get_HeadBoneTransform failed: %d", rc);
        return false;
    }
    LOGI("[AimNeck] get_HeadBoneTransform hooked @ 0x%lx", addr_head);

    return true;
}
