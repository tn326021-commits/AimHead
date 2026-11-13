# AimPatch — ARM64 / Dobby

Patch `.so` cho game FPS Unity IL2CPP.  
Tính năng: **AimNeck** | **NoRecoil + NoSpread** | **AntiBan**

---

## Offsets nguồn gốc

Tất cả offset lấy từ `offset.cs` (Assembly-CSharp.dll dump).

| Feature | Class / Method | Offset |
|---------|---------------|--------|
| get_NeckBone | `Player::get_NeckBone()` | `+0x708E850` |
| get_HeadBoneTransform | `Player::get_HeadBoneTransform()` | `+0x6F2F4B8` |
| get_SpineBoneTransform | `Player::get_SpineBoneTransform()` | `+0x6F25938` |
| enableRecoil (field) | `WeaponGunFeelProfileData` | `+0x214` (bool) |
| enableSpread (field) | `WeaponGunFeelProfileData` | `+0x21A` (bool) |
| DeserializeData | `WeaponGunFeelProfileData` | `+0x68423D8` |
| adsRecoilPitchMax | `WeaponGunFeelProfileData` | `+0x19C` (float) |
| hipSpreadLimit | `WeaponGunFeelProfileData` | `+0x16C` (float) |

---

## Build với GitHub Actions

1. Push repo lên GitHub
2. Vào **Actions** → **Build AimPatch ARM64** → **Run workflow**
3. Download artifact `libaimmod-arm64` → lấy `libaimmod.so`

Build tự động:
- NDK r25c (ARM64 / clang)
- Dobby build từ source (jmpews/Dobby)
- Output: `out/libs/arm64-v8a/libaimmod.so`

---

## Cấu trúc project

```
├── include/
│   ├── offsets.h          ← Tất cả offset từ offset.cs
│   ├── il2cpp_helpers.h   ← ReadField/WriteField/CallMethod utils
│   ├── aim_neck.h         ← Hook get_NeckBone + cache Transform
│   ├── no_recoil.h        ← Hook DeserializeData → zero recoil/spread
│   └── antiban.h          ← Hook fopen/fgets/ptrace
├── src/
│   └── main.cpp           ← JNI_OnLoad entry point
├── jni/
│   ├── Android.mk
│   └── Application.mk
└── .github/workflows/
    └── build.yml
```

---

## Cơ chế hoạt động

### AimNeck
- Hook `Player::get_NeckBone()` tại offset `0x708E850`
- Mỗi khi game gọi để lấy Transform cổ của player, ta cache pointer đó
- Dùng pointer này để tính toán góc ngắm về phía neck

### NoRecoil / NoSpread
- Hook `WeaponGunFeelProfileData::DeserializeData()` tại `0x68423D8`
- Sau khi game load dữ liệu vũ khí từ binary, ta ghi đè toàn bộ float recoil/spread về `0.0f`
- `enableRecoil = false`, `enableSpread = false`

### AntiBan
- Hook `fgets` → lọc bỏ dòng chứa `libaimmod.so` trong `/proc/self/maps`
- Hook `ptrace` → chặn PTRACE_ATTACH
- Chạy trước tất cả hook khác

---

## Lưu ý

- Chỉ dùng cho game **do bạn sở hữu**
- Test trên thiết bị ARM64 Android API 21+
- Nếu game update → recheck offset bằng IL2CPP dump mới
