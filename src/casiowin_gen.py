import yaml
import sys

TEMPLATE_code = """\
.global _CW_{name}
_CW_{name}:
    bra     _CASIOWIN_call
    mov     #{id}, r0\n
"""

def usage(v=None):
    print("usage: python casiowin_gen.py <INPUT.yaml> <OUTPUT.inc>")
    return v

def main(argv):
    if len(argv) != 3:
        return usage(1)
    if "--help" in argv:
        return usage(0)

    inp = argv[1]
    outp = argv[2]

    with open(inp, "r") as fp:
        data = yaml.safe_load(fp.read())

    api_versions = data["api_versions"]
    func = data["functions"]

    for name in func:
        l = len(func[name])
        if l != api_versions:
            print(f"warning: {name} has {l} version instead of {api_versions}")
            func[name] += [0] * (api_versions - l)
        if 0 in func[name]:
            print(f"warning: {name} is missing definitions in some versions")

    # Dictionary order is guaranteed by Python ≥ 3.7
    functions = list(enumerate(data["functions"].items()))

    with open(outp, "w") as fp:
        fp.write(f"#define CASIOWIN_API_VERSIONS {api_versions}\n\n")
        for i, (name, addresses) in functions:
            fp.write(TEMPLATE_code.format(id=i, name=name))

        fp.write("_CASIOWIN_TABLE:\n")
        for i, (name, addresses) in functions:
            longs = ", ".join(f"0x{a:<8x}" for a in addresses)
            fp.write(f"    .long {longs}  /* (#{i}) {name} */\n")

if __name__ == "__main__":
    sys.exit(main(sys.argv))
