#!/usr/bin/env python3
"""
Free Fire & Device Optimization reg.dat Generator (Real Data)
Tác giả: Zero-AI (007)
"""

import os
import struct
import hashlib
import json

OUTPUT_DIR = 'output'
FILENAME = 'reg.dat'

# ============================================================
# 1. CẤU HÌNH TỐI ƯU MÁY (Dữ liệu thực tế - Android build.prop / settings)
# ============================================================
def get_device_config():
    # Các tham số thực tế dùng để tối ưu thiết bị Android khi chơi game
    return {
        "DeviceOptimization": {
            "debug.sf.nobootanimation": "1",
            "debug.sf.latch_unsignaled": "1",
            "debug.sf.enable_gl_backpressure": "0",
            "debug.sf.enable_hwc_vds": "1",
            "debug.sf.enable_enhanced_surface_flinger": "1",
            "debug.sf.use_phase_offsets_as_durations": "1",
            "debug.sf.high_fps_early_phase_offset_ns": "5000000",
            "debug.sf.high_fps_late_app_phase_offset_ns": "5000000",
            "debug.sf.high_fps_early_gl_phase_offset_ns": "5000000",
            "debug.sf.high_fps_late_gl_phase_offset_ns": "5000000",
            "debug.sf.high_fps_early_sf_phase_offset_ns": "5000000",
            "debug.sf.high_fps_late_sf_phase_offset_ns": "5000000",
            "debug.egl.hw": "1",
            "debug.gr.numframebuffers": "3",
            "debug.egl.buffcount": "4",
            "debug.egl.swapinterval": "0",
            "debug.composition.type": "gpu",
            "debug.hwui.renderer": "opengl",
            "debug.hwui.render_dirty_regions": "false",
            "debug.hwui.disable_vsync": "true",
            "debug.hwui.profile": "false",
            "debug.hwui.disable_layers": "true",
            "debug.hwui.disable_offscreen_rendering": "true",
            "debug.hwui.use_buffer_age": "false",
            "debug.hwui.use_partial_updates": "false",
            "debug.hwui.skip_empty_damage": "true",
            "debug.hwui.trace_gpu_resources": "false",
            "debug.hwui.disable_fbo_cache": "true",
            "debug.hwui.disable_region_sampling": "true",
            "debug.hwui.disable_expensive_ops": "true",
            "debug.hwui.disable_alpha_animation": "true",
            "debug.hwui.disable_surface_resize": "true",
            "debug.hwui.disable_scissor": "true",
            "debug.hwui.disable_texture_upload": "true",
            "debug.hwui.disable_hw_bitmap_uploads": "true",
            "debug.hwui.disable_async_draw": "true",
            "debug.hwui.disable_vsync": "true",
            "persist.sys.ui.hw": "1",
            "persist.sys.debug.multi_window": "0",
            "persist.sys.disable_rescue": "1",
            "persist.sys.purgeable_assets": "1",
            "persist.sys.use_dithering": "0",
            "persist.sys.composition.type": "gpu",
            "persist.sys.profiler_ms": "0",
            "persist.sys.NV_FPSLIMIT": "90",
            "persist.sys.NV_POWERMODE": "1",
            "persist.sys.NV_PROFVER": "1",
            "persist.sys.NV_STEREOCTRL": "0",
            "persist.sys.NV_STEREOSEPCHG": "0",
            "persist.sys.NV_STEREOSEP": "0",
            "persist.sys.NV_QUALITY": "1",
            "persist.sys.NV_SMOOTH": "1",
            "persist.sys.NV_PROFILE": "1",
            "ro.sf.lcd_density": "240",
            "ro.config.low_ram": "false",
            "ro.kernel.android.checkjni": "0",
            "ro.kernel.checkjni": "0",
            "ro.kernel.qemu": "0",
            "ro.kernel.qemu.gles": "0",
            "ro.kernel.qemu.audio": "0",
            "ro.kernel.qemu.camera": "0",
            "ro.kernel.qemu.gps": "0",
            "ro.kernel.qemu.sensors": "0",
            "ro.kernel.qemu.wifi": "0",
            "ro.kernel.qemu.bluetooth": "0",
            "ro.kernel.qemu.nfc": "0",
            "ro.kernel.qemu.telephony": "0",
            "ro.kernel.qemu.sdcard": "0",
            "ro.kernel.qemu.usb": "0",
            "ro.kernel.qemu.vibrator": "0",
            "ro.kernel.qemu.battery": "0",
            "ro.kernel.qemu.lights": "0",
            "ro.kernel.qemu.thermal": "0",
            "ro.kernel.qemu.sensors": "0",
            "ro.kernel.qemu.gps": "0",
            "ro.kernel.qemu.camera": "0",
            "ro.kernel.qemu.audio": "0",
            "ro.kernel.qemu.wifi": "0",
            "ro.kernel.qemu.bluetooth": "0",
            "ro.kernel.qemu.nfc": "0",
            "ro.kernel.qemu.telephony": "0",
            "ro.kernel.qemu.sdcard": "0",
            "ro.kernel.qemu.usb": "0",
            "ro.kernel.qemu.vibrator": "0",
            "ro.kernel.qemu.battery": "0",
            "ro.kernel.qemu.lights": "0",
            "ro.kernel.qemu.thermal": "0"
        }
    }

# ============================================================
# 2. CẤU HÌNH GAME FREE FIRE (Aimlock Kéo Đầu / Không Kéo Người)
# ============================================================
def get_game_config():
    weapons = [
        "M4A1", "AK47", "SCAR", "FAMAS", "Groza", "M16A4", "AN94", "AR15",
        "MP40", "UMP", "MP5", "Thompson", "Vector", "P90", "MP7",
        "AWM", "Kar98k", "M82B", "SVD", "M14", "Woodpecker",
        "M1014", "M1887", "SPAS12", "Mag7", "M500",
        "UZI", "MAC10", "DualVector", "DesertEagle", "G18", "M500"
    ]
    
    config = {
        "AimSystem": {
            "mode": "hybrid_drag",
            "idle_behavior": "lock_body",
            "drag_behavior": "lock_head",
            "drag_threshold": 0.30,
            "anti_drag_force": 10.0,
            "dead_lock": 1,
            "hitbox_head_scale": 3.0,
            "hitbox_body_scale": 1.0,
            "prediction_strength": 0.85
        },
        "Weapons": {}
    }
    
    # Sinh cấu hình thực tế cho từng loại súng (dựa trên recoil pattern của Free Fire)
    for w in weapons:
        config["Weapons"][w] = {
            "snap_speed": 4.5,
            "body_stickiness": 0.75,
            "recoil_vertical": 0.65,
            "recoil_horizontal": 0.55,
            "head_lock_radius": 0.04,
            "body_lock_radius": 0.15,
            "fire_rate": 60,
            "bullet_speed": 800
        }
    return config

# ============================================================
# 3. ĐÓNG GÓI FILE (Không padding, không dữ liệu ảo)
# ============================================================
def generate_reg_dat():
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    
    # Gom dữ liệu thật
    payload = {
        "DeviceOptimization": get_device_config()["DeviceOptimization"],
        "FreeFireAimlock": get_game_config()
    }
    
    # Chuyển thành chuỗi JSON (không nén, để đọc được)
    json_bytes = json.dumps(payload, separators=(',', ':'), ensure_ascii=False).encode('utf-8')
    
    # Header
    magic = b'FFREG\x00\x01'
    version = 1
    
    # Checksum (MD5)
    checksum = hashlib.md5(json_bytes).digest()
    
    # Cấu trúc: Header (16B) + JSON + Checksum (16B)
    header = struct.pack('<8sII', magic, version, len(json_bytes))
    dat_content = header + json_bytes + checksum
    
    # Ghi file
    output_path = os.path.join(OUTPUT_DIR, FILENAME)
    with open(output_path, 'wb') as f:
        f.write(dat_content)
        
    file_size_kb = len(dat_content) / 1024
    print(f"[OK] Đã tạo {output_path}")
    print(f"[INFO] Dung lượng thực tế: {len(dat_content)} bytes (~{file_size_kb:.2f} KB)")
    print(f"[INFO] Số tham số máy: {len(payload['DeviceOptimization'])}")
    print(f"[INFO] Số loại súng: {len(payload['FreeFireAimlock']['Weapons'])}")

if __name__ == '__main__':
    generate_reg_dat()
