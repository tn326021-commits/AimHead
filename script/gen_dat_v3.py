#!/usr/bin/env python3
"""Generate single aimlockhead.dat config file."""
import struct, zlib, time, sys, os

MAGIC = b"AIMC"
VERSION = 3
T_FLOAT, T_INT, T_BOOL, T_DOUBLE = 0, 1, 2, 3

def build_config():
    return {
        "aim_lock_head": True,
        "sensitivity_general": 100,
        "sensitivity_red_dot": 92,
        "sensitivity_2x": 85,
        "sensitivity_4x": 75,
        "sensitivity_awm": 70,
        "sensitivity_free_look": 100,
        "touch_offset_x": 0.0,
        "touch_offset_y": -2.0,
        "fov": 100.0,
        "smoothing": 0.33,
        "max_distance": 200.0,
        "predict_time": 0.05,
        "head_offset": 1.70,
        "recoil_control": 0.42,
        "trigger_delay_ms": 30,
        "bone_id": 6,
        "fps_limit": 90,
        "graphics_quality": 2,
        "drag_enabled": True,
        "drag_strength": 0.55,
        "drag_smoothing": 0.40,
        "drag_radius": 180.0,
        "drag_deadzone": 12.0,
        "drag_vertical_bias": 0.75,
        "drag_max_speed": 22.0,
        "drag_lock_head": True,
        "drag_fov": 110.0,
        "drag_hold_ms": 40,
    }

def pack_entry(ki, v):
    if isinstance(v, bool):
        return struct.pack("<HB", ki, T_BOOL) + struct.pack("<B", 1 if v else 0) + b"\x00"*7
    if isinstance(v, int):
        return struct.pack("<HBi", ki, T_INT, v) + b"\x00"*4
    if isinstance(v, float):
        return struct.pack("<HBf", ki, T_FLOAT, v) + b"\x00"*4
    return struct.pack("<HBd", ki, T_DOUBLE, float(v))

def write_dat(out_path):
    cfg = build_config()
    keys = list(cfg.keys())
    kidx = {k: i for i, k in enumerate(keys)}

    st = struct.pack("<H", len(keys))
    for k in keys:
        b = k.encode("utf-8")
        st += struct.pack("<H", len(b)) + b

    ent = struct.pack("<H", len(keys))
    for k, v in cfg.items():
        ent += pack_entry(kidx[k], v)

    payload = st + ent
    crc = zlib.crc32(payload) & 0xFFFFFFFF
    flags = 0b00001111

    hdr = MAGIC
    hdr += struct.pack("<H", VERSION)
    hdr += struct.pack("<B", 1)
    hdr += struct.pack("<B", flags)
    hdr += struct.pack("<I", crc)
    hdr += struct.pack("<I", len(payload))
    hdr += struct.pack("<Q", int(time.time()))
    hdr += b"\x00" * 8

    os.makedirs(os.path.dirname(out_path) or ".", exist_ok=True)
    with open(out_path, "wb") as f:
        f.write(hdr); f.write(payload)
    print(f"[+] {out_path} ({len(hdr)+len(payload)} bytes)")

if __name__ == "__main__":
    out = sys.argv[1] if len(sys.argv) > 1 else "out/aimlockhead.dat"
    write_dat(out)
