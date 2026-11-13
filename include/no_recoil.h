#pragma once
#include "il2cpp_helpers.h"
#include "offsets.h"
#include <dobby.h>

// ----------------------------------------------------------------
// No Recoil + No Spread
// Target: WeaponGunFeelProfileData (CSVBaseData)
// Strategy: Hook the method that loads/applies GunFeelProfileData
//           then zero all recoil & spread fields on the live object.
// ----------------------------------------------------------------

static uintptr_t g_base = 0;

// Original function pointer saved by Dobby
// WeaponGunFeelProfileData::DeserializeData(MutableStringBinaryReader bR)
// Offset: 0x68423D8  — confirmed from offset.cs line 259977
static void (*orig_WeaponGunFeelProfileData_DeserializeData)(void* self, void* bR) = nullptr;

// Hook: fires after each weapon profile row is deserialized from binary.
// We overwrite every recoil/spread float on the object to 0.
static void hook_WeaponGunFeelProfileData_DeserializeData(void* self, void* bR)
{
    // Call original first so all fields are properly populated
    orig_WeaponGunFeelProfileData_DeserializeData(self, bR);
    void* ret = nullptr;

    if (!self) return ret;

    // --- Disable recoil booleans ---
    WriteField<uint8_t>(self, Offsets::enableRecoil,       0);
    WriteField<uint8_t>(self, Offsets::enableVisualRecoil, 0);
    WriteField<uint8_t>(self, Offsets::enableSpread,       0);

    // --- Zero ADS recoil magnitudes ---
    WriteField<float>(self, Offsets::adsRecoilPitchMax,   0.0f);
    WriteField<float>(self, Offsets::adsRecoilPitchMin,   0.0f);
    WriteField<float>(self, Offsets::adsRecoilPitchScale, 0.0f);
    WriteField<float>(self, Offsets::adsRecoilPitchNoise, 0.0f);
    WriteField<float>(self, Offsets::adsRecoilYawMax,     0.0f);
    WriteField<float>(self, Offsets::adsRecoilYawMin,     0.0f);
    WriteField<float>(self, Offsets::adsRecoilYawScale,   0.0f);
    WriteField<float>(self, Offsets::adsRecoilYawNoise,   0.0f);

    // --- Zero Hip-fire recoil ---
    WriteField<float>(self, Offsets::hipRecoilPitchMax,   0.0f);
    WriteField<float>(self, Offsets::hipRecoilPitchMin,   0.0f);
    WriteField<float>(self, Offsets::hipRecoilPitchScale, 0.0f);
    WriteField<float>(self, Offsets::hipRecoilYawMax,     0.0f);
    WriteField<float>(self, Offsets::hipRecoilYawMin,     0.0f);
    WriteField<float>(self, Offsets::hipRecoilYawScale,   0.0f);

    // --- Zero Spread ---
    WriteField<float>(self, Offsets::adsSpreadLimit,      0.0f);
    WriteField<float>(self, Offsets::hipSpreadLimit,      0.0f);
    WriteField<float>(self, Offsets::adsSpreadClampMax,   0.0f);
    WriteField<float>(self, Offsets::adsSpreadClampMin,   0.0f);
    WriteField<float>(self, Offsets::hipSpreadClampMax,   0.0f);
    WriteField<float>(self, Offsets::hipSpreadClampMin,   0.0f);
    WriteField<float>(self, Offsets::adsSpreadStart,      0.0f);
    WriteField<float>(self, Offsets::hipSpreadStart,      0.0f);
    WriteField<float>(self, Offsets::adsSpreadStepPerShot,0.0f);
    WriteField<float>(self, Offsets::hipSpreadStepPerShot,0.0f);

    // --- Zero return/reset timing to snap back instantly ---
    WriteField<float>(self, Offsets::recoilReturnRatio,   1.0f); // full return
    WriteField<float>(self, Offsets::recoilReturnDelay,   0.0f);
    WriteField<float>(self, Offsets::recoilReturnTime,    0.0f);
    WriteField<float>(self, Offsets::recoilShotResetTime, 0.0f);
    WriteField<float>(self, Offsets::crouchRecoilScale,   0.0f);
    WriteField<float>(self, Offsets::visualFollowRealRecoil, 0.0f);

}

// WeaponGunFeelProfileData::DeserializeData — offset.cs line 259977
// Offset : 0x68423D8
static constexpr uintptr_t OFFSET_DeserializeData_WeaponGunFeel = 0x68423D8;

inline bool InstallNoRecoil(uintptr_t base) {
    uintptr_t addr = base + OFFSET_DeserializeData_WeaponGunFeel;

    int rc = DobbyHook(
        reinterpret_cast<void*>(addr),
        reinterpret_cast<void*>(hook_WeaponGunFeelProfileData_DeserializeData),
        reinterpret_cast<void**>(&orig_WeaponGunFeelProfileData_DeserializeData)
    );

    if (rc != RS_SUCCESS) {
        LOGE("[NoRecoil] DobbyHook failed: %d", rc);
        return false;
    }

    LOGI("[NoRecoil] Hook installed @ 0x%lx (+0x%lx)",
         addr, OFFSET_DeserializeData_WeaponGunFeel);
    return true;
}
