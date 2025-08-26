#!/usr/bin/env python3

import sys
from pathlib import Path
from PIL import Image

def convert_image(path):
    img = Image.open(path)
    w, h = img.size
    transp = 0xff
    if "transparency" in img.info:
        transp = img.info["transparency"]

    pixels = []
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
        f.write(f"    0xf0, 0x34, 0x01, 0x{transp:02x}, 0x{(w & 0xff):02x}, 0x{(w >> 8):02x}, 0x{(h & 0xff):02x}, 0x{(h >> 8):02x},\n")
        for i in range(0, len(pixels), 2):
            if i % 32 == 0:
                f.write("    ")
            f.write(f"0x{pixels[i+1]:01x}{pixels[i]:01x}, ")
            if i % 32 == 30:
                f.write("\n")
        f.write("};\n\n")
        f.write("#endif\n")

if __name__ == "__main__":
    if len(sys.argv) <= 1:
        print(f"Usage {sys.argv[0]} IMAGE_PATH")
        sys.exit(1)
    convert_image(sys.argv[1])
