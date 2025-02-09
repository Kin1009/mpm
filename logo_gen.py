from PIL import Image
import sys

TEMPLATE_code = """\
#define LOGO_WIDTH {width}
#define LOGO_HEIGHT {height}
static u16 const logo[{size}] = {{ {data} }};
"""

def usage(v=None):
    print("usage: python logo_gen.py <INPUT.png> <OUTPUT.inc>")
    return v

def main(argv):
    if len(argv) != 3:
        return usage(1)
    if "--help" in argv:
        return usage(0)

    inp = argv[1]
    outp = argv[2]

    img = Image.open(inp).convert("RGB")
    w, h = img.size
    px = img.load()

    values = []
    for y in range(h):
        for x in range(w):
            r, g, b = px[x, y]
            r = (r >> 3) & 0x1f
            g = (g >> 2) & 0x3f
            b = (b >> 3) & 0x1f
            values.append((r << 11) + (g << 5) + b)

    size = len(values)
    data = ", ".join(hex(v) for v in values)
    with open(outp, "w") as fp:
        fp.write(TEMPLATE_code.format(width=w, height=h, size=size, data=data))

if __name__ == "__main__":
    sys.exit(main(sys.argv))
