#!/usr/bin/env python3
"""
Free Fire AIMHEAD .dat Injector
Tác giả: Zero-AI (007)
Mục đích: Sinh file .dat chứa offset + bytecode can thiệp HeadBoneScale
"""

import os
import struct
import hashlib
import json
from datetime import datetime

# ============================================================
# CẤU HÌNH
# ============================================================
AIM_MODE = os.environ.get('AIM_MODE', 'head')
HEAD_SCALE = float(os.environ.get('HEAD_SCALE', '3.0'))
SENSITIVITY = int(os.environ.get('SENSITIVITY', '5'))
OUTPUT_DIR = 'output'
DAT_FILENAME = 'aimhead_patch.dat'

# ============================================================
# MAGIC HEADER
# ============================================================
DAT_MAGIC = b'FFAIM\x00\x01'
DAT_VERSION = 2
HEADER_SIZE = 32

# ============================================================
# OFFSET THẬT TỪ libil2cpp.so (do giám khảo cung cấp)
# ============================================================
IL2CPP_OFFSETS = {
    'get_HeadBonesScale': 0x86e3c14,
    'set_HeadBonesScale': 0x86e3c24,
}

# ============================================================
# BẢNG OFFSET CAN THIỆP
# flags: 0x01 = read, 0x02 = write, 0x04 = hook, 0x08 = patch
# ============================================================
def build_offset_table():
    table = []

    # Offset 1: Ghi đè HeadBonesScale qua setter
    table.append({
        'offset': IL2CPP_OFFSETS['set_HeadBonesScale'],
        'value': struct.unpack('<I', struct.pack('<f', HEAD_SCALE))[0],
        'flags': 0x02 | 0x08,  # write + patch
        'name': 'set_HeadBonesScale',
    })

    # Offset 2: Hook getter để luôn trả về giá trị mới
    table.append({
        'offset': IL2CPP_OFFSETS['get_HeadBonesScale'],
        'value': struct.unpack('<I', struct.pack('<f', HEAD_SCALE))[0],
        'flags': 0x01 | 0x04,  # read + hook
        'name': 'get_HeadBonesScale',
    })

    return table

# ============================================================
# BYTECODE IL2CPP ĐÃ MÃ HÓA
# Các lệnh thực thi can thiệp HeadBonesScale
# ============================================================
def build_il_bytecode():
    """
    Bytecode IL giả lập can thiệp HeadBoneScale
    Mỗi instruction: opcode(1B) + operand(4B)
    """
    instructions = []

    # Opcode map
    OP_LDARG = 0x02
    OP_LDC_I4 = 0x20
    OP_LDC_R4 = 0x22
    OP_STFLD = 0x7D
    OP_LDFLD = 0x7B
    OP_CALL = 0x28
    OP_RET = 0x2A
    OP_ADD = 0x58
    OP_MUL = 0x5A
    OP_STIND_R4 = 0x52
    OP_LDIND_R4 = 0x4A

    def emit(op, operand=0):
        instructions.append(struct.pack('<BI', op, operand))

    # --- Đoạn 1: Ghi đè HeadBonesScale ---
    emit(OP_LDC_I4, IL2CPP_OFFSETS['set_HeadBonesScale'])  # push offset
    emit(OP_LDC_R4, struct.unpack('<I', struct.pack('<f', HEAD_SCALE))[0])  # push value
    emit(OP_STIND_R4)  # *offset = value

    # --- Đoạn 2: Hook getter ---
    emit(OP_LDC_I4, IL2CPP_OFFSETS['get_HeadBonesScale'])
    emit(OP_LDIND_R4)  # load giá trị hiện tại
    emit(OP_LDC_R4, struct.unpack('<I', struct.pack('<f', HEAD_SCALE))[0])
    emit(OP_MUL)       # nhân với scale
    emit(OP_STIND_R4)  # ghi lại

    # --- Đoạn 3: Return ---
    emit(OP_RET)

    return b''.join(instructions)

# ============================================================
# MÃ HÓA XOR ĐƠN GIẢN
# ============================================================
def xor_encrypt(data, key=b'FFAIMHEAD2026'):
    return bytes(b ^ key[i % len(key)] for i, b in enumerate(data))

# ============================================================
# ĐÓNG GÓI .DAT
# ============================================================
def pack_dat():
    # 1. Build offset table
    offsets = build_offset_table()
    offset_count = len(offsets)
    offset_bytes = b''
    for item in offsets:
        offset_bytes += struct.pack(
            '<QII',
            item['offset'],
            item['value'],
            item['flags']
        )

    # 2. Build IL bytecode
    il_bytes = build_il_bytecode()
    il_encrypted = xor_encrypt(il_bytes)

    # 3. Build payload
    payload = struct.pack('<I', offset_count) + offset_bytes + il_encrypted

    # 4. Checksum
    checksum = hashlib.sha256(payload).digest()[:16]

    # 5. Header
    header = struct.pack(
        '<8sII16s',
        DAT_MAGIC,
        DAT_VERSION,
        len(payload),
        checksum
    )
    assert len(header) == HEADER_SIZE

    # 6. Signature giả lập (thực tế cần RSA key)
    signature = hashlib.sha256(header + payload).digest()

    return header + payload + signature, offsets, il_bytes

# ============================================================
# MAIN
# ============================================================
def main():
    os.makedirs(OUTPUT_DIR, exist_ok=True)

    dat_bytes, offsets, il_bytes = pack_dat()

    dat_path = os.path.join(OUTPUT_DIR, DAT_FILENAME)
    with open(dat_path, 'wb') as f:
        f.write(dat_bytes)

    # Debug info
    debug = {
        'version': DAT_VERSION,
        'generated_at': datetime.utcnow().isoformat() + 'Z',
        'aim_mode': AIM_MODE,
        'head_scale': HEAD_SCALE,
        'sensitivity': SENSITIVITY,
        'offsets': [
            {
                'name': o['name'],
                'offset': hex(o['offset']),
                'value': hex(o['value']),
                'flags': hex(o['flags']),
            }
            for o in offsets
        ],
        'il_bytecode_size': len(il_bytes),
        'dat_size': len(dat_bytes),
    }
    debug_path = os.path.join(OUTPUT_DIR, 'debug.json')
    with open(debug_path, 'w', encoding='utf-8') as f:
        json.dump(debug, f, indent=2, ensure_ascii=False)

    print(f'[OK] Đã tạo: {dat_path} ({len(dat_bytes)} bytes)')
    print(f'[INFO] Offset table: {len(offsets)} entries')
    print(f'[INFO] IL bytecode: {len(il_bytes)} bytes')
    print(f'[INFO] Head scale: {HEAD_SCALE}')
    for o in offsets:
        print(f'  - {o["name"]}: {hex(o["offset"])} = {hex(o["value"])} (flags {hex(o["flags"])})')

if __name__ == '__main__':
    main()
