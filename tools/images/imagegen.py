#!/usr/bin/env python3

import sys
from pathlib import Path
from PIL import Image

def convert_image(path):
    img = Image.open(path)
    w, h = img.size
    pixels = [0xf0, 0x34, 0x01, 0x00, w & 0xff, w >> 8, h & 0xff, h >> 8]
    for y in range(h):
        for x in range(w):
            pixels.append(img.getpixel((x, y)))
    basename = Path(path).stem
    with open(basename + ".h", "w") as f:
        f.write(f"#ifndef {basename.upper()}_IMAGE_H\n")
        f.write(f"#define {basename.upper()}_IMAGE_H\n\n")
        f.write("#include <stdint.h>\n\n")
        f.write("#ifdef FIRMWARE\n")
        f.write("  #include <pico.h>\n")
        f.write("  #define IN_FLASH __in_flash()\n")
        f.write("#else\n")
        f.write("  #define IN_FLASH\n")
        f.write("#endif\n\n")

        # Write image data
        f.write(f"static const uint8_t IN_FLASH {basename}_image[] = {{\n")
        for i in range(len(pixels)):
            if i % 16 == 0:
                f.write("    ")
            f.write(f"0x{pixels[i]:02x}, ")
            if i % 16 == 15:
                f.write("\n")
        f.write("};\n\n")
        f.write("#endif\n")

if __name__ == "__main__":
    if len(sys.argv) <= 1:
        print(f"Usage {sys.argv[0]} IMAGE_PATH")
        sys.exit(1)
    convert_image(sys.argv[1])
