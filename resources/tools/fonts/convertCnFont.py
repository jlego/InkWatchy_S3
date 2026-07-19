#!/usr/bin/env python3
"""
Chinese font converter for InkWatchy (sparse encoding).
Only stores actually used characters, not the full Unicode range.
Supports TTF, OTF, and TTC font files.

Binary format:
  [2 bytes] bitmap_size
  [bitmap_size bytes] bitmap data
  [2 bytes] glyph_count (N)
  [N * 8 bytes] GFXglyph array (8 bytes each)
  [N * 2 bytes] codepoint array (uint16_t, sorted)
  [1 byte] yAdvance

Usage:
    python3 convertCnFont.py <font_file> <font_size> <output_path> [ttc_face_index]

Requires: pip3 install freetype-py
"""

import sys
import os
import struct
import re

try:
    import freetype
except ImportError:
    print("Error: freetype-py not installed. Run: pip3 install freetype-py")
    sys.exit(1)

EXTRA_CHARS = "　、。·…—‘’“”【】！（），．：；？◎○●■□←↑→↓×÷℃℡ⅠⅡⅢⅣⅤⅥⅦⅧⅨⅩ°‰☉☎☔☕☝☺♈♉♊♋♌♍♎♏♐♑♒♓♠♡♢♣♤♥♦♧♨♩♪♫♬♭♮♯♰♱♲♳♾♿⚀⚁⚂⚃⚄⚅⚆⚇⚈⚉⚊⚋⚌⚍⚎⚏⚐⚑⚒⚓⚔⚕⚖⚗⚘⚙⚚⚛⚜⚝⚞⚟⚠⚡⚢⚣⚤⚥⚦⚧⚨⚩⚪⚫⚬⚭⚮⚯⚰⚱⚲⚳⚴⚵⚶⚷⚸⚹⚺⚻⚼⚽⚾⚿⛀⛁⛂⛃⛄⛅⛆⛇⛈⛉⛊⛋⛌⛍⛎⛏⛐⛑⛒⛓⛔⛕⛖⛗⛘⛙⛚⛛⛜⛝⛞⛟⛠⛡⛢⛣⛤⛥⛦⛧⛨⛩⛪⛫⛬⛭⛮⛯⛰⛱⛲⛳⛴⛵⛶⛷⛸⛹⛺⛻⛼⛽⛾⛿"

def extract_chinese_chars_from_file(filepath):
    chars = set()
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
    except (UnicodeDecodeError, FileNotFoundError):
        return chars
    for ch in content:
        if ord(ch) > 0x7F:
            chars.add(ch)
    return chars

def extract_chinese_chars(src_dir):
    chars = set()
    for ch in EXTRA_CHARS:
        if ord(ch) > 0x7F:
            chars.add(ch)
    if not os.path.exists(src_dir):
        return chars
    for root, dirs, files in os.walk(src_dir):
        for fname in files:
            if fname.endswith(('.h', '.cpp', '.c', '.hpp')):
                fpath = os.path.join(root, fname)
                chars |= extract_chinese_chars_from_file(fpath)
    return chars

def extract_book_chars(script_dir):
    chars = set()
    book_dirs = [
        os.path.join(script_dir, '..', 'fs', 'littlefs', 'book'),
        os.path.join(script_dir, '..', 'books', 'out'),
    ]
    for book_dir in book_dirs:
        if not os.path.exists(book_dir):
            continue
        for root, dirs, files in os.walk(book_dir):
            for fname in files:
                fpath = os.path.join(root, fname)
                chars |= extract_chinese_chars_from_file(fpath)
    return chars

def gb2312_chars():
    chars = set()
    for hi in range(0xB0, 0xF8):
        for lo in range(0xA1, 0xFF):
            try:
                b = bytes([hi, lo])
                s = b.decode('gb2312')
                for ch in s:
                    if ord(ch) > 0x7F:
                        chars.add(ch)
            except (UnicodeDecodeError, ValueError):
                pass
    for hi in range(0xA1, 0xA8):
        for lo in range(0xA1, 0xFF):
            try:
                b = bytes([hi, lo])
                s = b.decode('gb2312')
                for ch in s:
                    if ord(ch) > 0x7F:
                        chars.add(ch)
            except (UnicodeDecodeError, ValueError):
                pass
    return chars

def render_glyph(face, char_code, size):
    face.set_char_size(size * 64)
    face.load_char(char_code, freetype.FT_LOAD_RENDER | freetype.FT_LOAD_TARGET_MONO)
    glyph = face.glyph
    bitmap = glyph.bitmap
    w = bitmap.width
    h = bitmap.rows
    if w == 0 or h == 0:
        return None
    bitmap_data = bytearray()
    byte_val = 0
    bit_idx = 0
    for y in range(h):
        for x in range(w):
            row_offset = y * bitmap.pitch
            if row_offset + x // 8 < len(bitmap.buffer):
                pixel = bitmap.buffer[row_offset + x // 8]
                if pixel & (0x80 >> (x % 8)):
                    byte_val |= (0x80 >> bit_idx)
            bit_idx += 1
            if bit_idx >= 8:
                bitmap_data.append(byte_val)
                byte_val = 0
                bit_idx = 0
    if bit_idx > 0:
        bitmap_data.append(byte_val)
    return {
        'width': w,
        'height': h,
        'x_offset': glyph.bitmap_left,
        'y_offset': -glyph.bitmap_top,
        'x_advance': glyph.advance.x // 64,
        'bitmap': bytes(bitmap_data),
    }

def get_y_advance(face, font_size):
    try:
        return face.size.metrics.height // 64
    except AttributeError:
        pass
    try:
        return face.size.height // 64
    except AttributeError:
        pass
    return font_size + 2

def convert_font(font_path, font_size, output_path, ttc_index=0, full_gb2312=False):
    script_dir = os.path.dirname(os.path.abspath(__file__))
    src_dir = os.path.join(script_dir, '..', '..', '..', 'src')

    chars = extract_chinese_chars(src_dir)
    print(f"Extracted {len(chars)} unique Chinese characters from src/")

    book_chars = extract_book_chars(script_dir)
    chars |= book_chars
    print(f"After scanning books: {len(chars)} unique characters")

    if full_gb2312:
        gb_chars = gb2312_chars()
        chars |= gb_chars
        print(f"After adding full GB2312: {len(chars)} unique characters")
    if not chars:
        print("No Chinese characters found!")
        return

    codepoints = sorted(set(ord(c) for c in chars))
    print(f"Codepoints: {len(codepoints)} (range U+{codepoints[0]:04X} - U+{codepoints[-1]:04X})")

    face = None
    last_err = None
    load_args = [
        (font_path, -1),
        (font_path, 0),
    ]
    script_dir = os.path.dirname(os.path.abspath(__file__))
    fallback_fonts = [
        os.path.join(script_dir, 'cn', 'NotoSansSC-Regular.ttf'),
        os.path.join(script_dir, 'cn', 'Songti.ttc'),
    ]
    for bf in fallback_fonts:
        if os.path.abspath(bf) != os.path.abspath(font_path):
            load_args.append((bf, -1))
            load_args.append((bf, 0))
            load_args.append((bf, 1))
    for fp, idx in load_args:
        if not os.path.exists(fp):
            continue
        try:
            if idx < 0:
                face = freetype.Face(fp)
            else:
                face = freetype.Face(fp, idx)
            try:
                if hasattr(face, 'set_var_design_coordinates'):
                    face.set_var_design_coordinates(freetype.Vector([400]))
                    print(f"Set variable font weight to 400 (design coords)")
                elif hasattr(face, 'set_var_blend_coordinates'):
                    face.set_var_blend_coordinates(freetype.Vector([0.0]))
                    print(f"Set variable font weight to default (blend coords)")
            except Exception as e:
                print(f"Variable font weight set failed: {e}, using default")
            print(f"Loaded font: {fp} face={idx}")
            break
        except Exception as e:
            last_err = e
            print(f"Failed to load {fp} face={idx}: {e}")
            continue
    if face is None:
        print(f"Failed to load any font. Last error: {last_err}")
        sys.exit(1)

    face.set_char_size(font_size * 64)
    y_advance = get_y_advance(face, font_size)
    if y_advance <= 0:
        y_advance = font_size + 2

    all_bitmap = bytearray()
    glyphs = []
    used_codepoints = []
    bitmap_offset = 0
    skipped = 0

    for cp in codepoints:
        result = render_glyph(face, cp, font_size)
        if result is None:
            skipped += 1
            continue
        glyphs.append({
            'bitmap_offset': bitmap_offset,
            'width': result['width'],
            'height': result['height'],
            'x_advance': result['x_advance'],
            'x_offset': result['x_offset'],
            'y_offset': result['y_offset'],
        })
        all_bitmap.extend(result['bitmap'])
        bitmap_offset += len(result['bitmap'])
        used_codepoints.append(cp)

    glyph_count = len(glyphs)
    bitmap_size = len(all_bitmap)
    total_size = 4 + bitmap_size + 2 + glyph_count * 10 + glyph_count * 2 + 1

    print(f"Bitmap: {bitmap_size} bytes, Glyphs: {glyph_count}, Skipped: {skipped}, Total: {total_size} bytes")

    os.makedirs(os.path.dirname(output_path) if os.path.dirname(output_path) else '.', exist_ok=True)

    with open(output_path, 'wb') as f:
        f.write(struct.pack('<I', bitmap_size))
        f.write(bytes(all_bitmap))
        f.write(struct.pack('<H', glyph_count))
        for g in glyphs:
            f.write(struct.pack('<I', g['bitmap_offset']))
            f.write(struct.pack('<BBBbb', g['width'], g['height'], g['x_advance'], g['x_offset'], g['y_offset']))
            f.write(b'\x00')
        for cp in used_codepoints:
            f.write(struct.pack('<H', cp))
        f.write(struct.pack('<B', y_advance))

    print(f"Font written to: {output_path}")

if __name__ == '__main__':
    if len(sys.argv) < 4:
        print("Usage: python3 convertCnFont.py <font_file> <font_size> <output_path> [ttc_face_index] [--full-gb2312]")
        print("Example: python3 convertCnFont.py NotoSansSC-Regular.ttf 13 out/NotoSansSC13")
        print("         python3 convertCnFont.py Songti.ttc 13 out/NotoSansSC13 3")
        print("         python3 convertCnFont.py cn/NotoSansSC-Regular.ttf 16 out/NotoSansSC16 --full-gb2312")
        sys.exit(1)
    font_file = sys.argv[1]
    font_size = int(sys.argv[2])
    output_path = sys.argv[3]
    full_gb2312 = '--full-gb2312' in sys.argv
    ttc_index = 0
    for arg in sys.argv[4:]:
        if arg == '--full-gb2312':
            continue
        try:
            ttc_index = int(arg)
        except ValueError:
            pass
    if not os.path.exists(font_file):
        print(f"Error: Font file not found: {font_file}")
        sys.exit(1)
    convert_font(font_file, font_size, output_path, ttc_index, full_gb2312)