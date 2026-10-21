#!/usr/bin/env python3
"""
Zero-AI (007) - Unity Asset Bundle Extractor (CLI)
Tương thích GitHub Actions - Không cần GUI
"""
import os
import sys
import json
from pathlib import Path

# Cố gắng import thư viện
try:
    import UnityPy
    from PIL import Image
except ImportError:
    print("[!] Thiếu thư viện. Hãy chạy: pip install UnityPy Pillow")
    sys.exit(1)

# Cấu hình đường dẫn
ASSETS_DIR = 'assets'
OUTPUT_DIR = 'output'
REPORT_FILE = os.path.join(OUTPUT_DIR, 'extraction_report.json')

# Các định dạng file bundle cần quét
BUNDLE_EXTENSIONS = {'.bundle', '.asset', '.unity3d', '.dat', '.ab'}

def extract_file(filepath):
    """Trích xuất dữ liệu từ một file asset"""
    filename = os.path.basename(filepath)
    out_folder = os.path.join(OUTPUT_DIR, filename)
    os.makedirs(out_folder, exist_ok=True)
    
    stats = {'file': filename, 'status': 'success', 'extracted': 0, 'types': {}}
    
    try:
        env = UnityPy.load(filepath)
    except Exception as e:
        stats['status'] = f'error: {str(e)}'
        print(f"  [!] Lỗi load file (có thể do mã hóa): {e}")
        return stats

    for obj in env.objects:
        obj_type = obj.type.name
        try:
            data = obj.read()
            name = getattr(data, 'm_Name', f"unnamed_{obj.path_id}")
            
            # Làm sạch tên file
            safe_name = "".join([c for c in name if c.isalpha() or c.isdigit() or c in (' ', '-', '_')]).strip()
            if not safe_name:
                safe_name = f"object_{obj.path_id}"

            type_dir = os.path.join(out_folder, obj_type)
            os.makedirs(type_dir, exist_ok=True)

            # 1. Xử lý Hình ảnh (Texture2D / Sprite)
            if obj_type in ('Texture2D', 'Sprite'):
                img = data.image
                if img:
                    img.save(os.path.join(type_dir, f"{safe_name}.png"))
                    stats['extracted'] += 1
                    
            # 2. Xử lý Text / JSON / XML
            elif obj_type == 'TextAsset':
                if hasattr(data, 'text') and data.text:
                    with open(os.path.join(type_dir, f"{safe_name}.txt"), 'w', encoding='utf-8', errors='ignore') as f:
                        f.write(data.text)
                    stats['extracted'] += 1
                    
            # 3. Xử lý Âm thanh
            elif obj_type == 'AudioClip':
                if hasattr(data, 'samples'):
                    with open(os.path.join(type_dir, f"{safe_name}.wav"), 'wb') as f:
                        f.write(data.samples)
                    stats['extracted'] += 1

            # Cập nhật thống kê
            stats['types'][obj_type] = stats['types'].get(obj_type, 0) + 1

        except Exception as e:
            print(f"  [!] Lỗi extract {obj_type}: {e}")

    print(f"  [OK] Đã trích xuất {stats['extracted']} objects từ {filename}")
    return stats

def main():
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    
    if not os.path.exists(ASSETS_DIR):
        print(f"[!] Không tìm thấy thư mục '{ASSETS_DIR}' trên repo.")
        # Tạo thư mục rỗng để workflow không bị lỗi
        os.makedirs(ASSETS_DIR, exist_ok=True)
        return

    files = [str(f) for f in Path(ASSETS_DIR).rglob('*') 
             if f.is_file() and f.suffix.lower() in BUNDLE_EXTENSIONS]
             
    print(f"[*] Tìm thấy {len(files)} file bundle cần xử lý.")
    
    report = []
    for f in files:
        print(f"\n[*] Đang xử lý: {os.path.basename(f)}")
        report.append(extract_file(f))
        
    with open(REPORT_FILE, 'w', encoding='utf-8') as f:
        json.dump(report, f, indent=2, ensure_ascii=False)
    print(f"\n[OK] Đã lưu báo cáo tại {REPORT_FILE}")

if __name__ == '__main__':
    main()
