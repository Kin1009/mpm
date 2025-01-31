send:
	@ fxsdk build-cg
	fxlink -sw build-cg/mpm.bin

dis:
	@ fxsdk build-cg
	sh-elf-objdump -b binary -m sh4-nofpu -D build-cg/mpm.bin | tail -n +7
