#pragma once
#include <cstdint>

// ============================================================
// AUTO-GENERATED FROM offset.cs
// Game: Assembly-CSharp.dll (IL2CPP / Unity)
// Target: ARM64 | Dobby Hook Framework
// ============================================================

namespace Offsets {

    // ---- BONE TRANSFORM GETTERS (Method Offsets) ----
    // class Player (GamePlay module)
    static constexpr uintptr_t get_NeckBone         = 0x708E850;  // Transform get_NeckBone()
    static constexpr uintptr_t get_HeadBoneTransform = 0x6F2F4B8; // Transform get_HeadBoneTransform()
    static constexpr uintptr_t get_SpineBoneTransform = 0x6F25938; // Transform get_SpineBoneTransform()
    static constexpr uintptr_t get_HipsBoneTransform  = 0x6F2F250; // Transform get_HipsBoneTransform()

    // ---- HIT PART ENUM VALUES ----
    // enum HitPart (used by SABoneCollider system)
    // Head=0, Neck=1, Chest=2, Hips=3
    static constexpr int HITPART_HEAD  = 0;
    static constexpr int HITPART_NECK  = 1;
    static constexpr int HITPART_CHEST = 2;

    // ---- WEAPON GUNFEEL PROFILE DATA ----
    // class WeaponGunFeelProfileData : CSVBaseData
    // All offsets are FIELD offsets from object base
    static constexpr uintptr_t enableRecoil         = 0x214; // Boolean
    static constexpr uintptr_t enableVisualRecoil    = 0x215; // Boolean
    static constexpr uintptr_t enableSpread          = 0x21A; // Boolean

    static constexpr uintptr_t adsRecoilPitchMax     = 0x19C; // Single
    static constexpr uintptr_t adsRecoilPitchMin     = 0x1DC; // Single
    static constexpr uintptr_t adsRecoilPitchScale   = 0x1A8; // Single
    static constexpr uintptr_t adsRecoilPitchNoise   = 0x190; // Single
    static constexpr uintptr_t adsRecoilYawScale     = 0x1D8; // Single
    static constexpr uintptr_t adsRecoilYawMax       = 0x1E0; // Single
    static constexpr uintptr_t adsRecoilYawMin       = 0x198; // Single
    static constexpr uintptr_t adsRecoilYawNoise     = 0x18C; // Single

    static constexpr uintptr_t hipRecoilPitchMax     = 0x0BC; // Single
    static constexpr uintptr_t hipRecoilPitchMin     = 0x124; // Single
    static constexpr uintptr_t hipRecoilPitchScale   = 0x1E8; // Single
    static constexpr uintptr_t hipRecoilYawMax       = 0x1B4; // Single
    static constexpr uintptr_t hipRecoilYawMin       = 0x194; // Single
    static constexpr uintptr_t hipRecoilYawScale     = 0x0E4; // Single

    static constexpr uintptr_t adsSpreadLimit        = 0x140; // Single
    static constexpr uintptr_t hipSpreadLimit        = 0x16C; // Single
    static constexpr uintptr_t adsSpreadClampMax     = 0x12C; // Single
    static constexpr uintptr_t adsSpreadClampMin     = 0x130; // Single
    static constexpr uintptr_t hipSpreadClampMax     = 0x158; // Single
    static constexpr uintptr_t hipSpreadClampMin     = 0x15C; // Single
    static constexpr uintptr_t adsSpreadStart        = 0x144; // Single
    static constexpr uintptr_t hipSpreadStart        = 0x170; // Single
    static constexpr uintptr_t adsSpreadStepPerShot  = 0x13C; // Single
    static constexpr uintptr_t hipSpreadStepPerShot  = 0x168; // Single

    static constexpr uintptr_t recoilReturnRatio     = 0x208; // Single
    static constexpr uintptr_t recoilReturnDelay     = 0x134; // Single
    static constexpr uintptr_t recoilReturnTime      = 0x148; // Single
    static constexpr uintptr_t recoilShotResetTime   = 0x1E4; // Single
    static constexpr uintptr_t visualFollowRealRecoil= 0x08C; // Single
    static constexpr uintptr_t crouchRecoilScale     = 0x0B8; // Single

} // namespace Offsets
