.PHONY: build send dis

FAKE := $(HOME)/Desktop/mpm/casio-toolchain/home/.local
SYSROOT := $(FAKE)/share/fxsdk/sysroot

export PATH := $(FAKE)/bin:$(SYSROOT)/bin:$(PATH)

build:
	@fxsdk build-cg

send: build
	fxlink -sw build-cg/mpm.bin

dis: build
	sh-elf-objdump -b binary -m sh4-nofpu -D build-cg/mpm.bin | tail -n +7