# MPM: Add-in loader for Math+ programming

This repo contains the sources for `mpm.bin` (and `mpm.bin` itself), the main program in the Math+ Mod (MPM) which finds add-ins, displays the add-in menu, and loads the programs.

Currently compatible with Math+ OS 2.00.

## How to use

TODO: Photos and explanations on how to use.

## How to build

Compiles with the fxSDK (just for the toolchain, this doesn't use gint).

```bash
% fxsdk build-cg # Compile
% fxlink -sw build-cg/mpm.bin # Send to calculator
```

## How to build the toolchain

Rename casio-toolchain-stub to casio-toolchain-finish.
Download the patches folder in the Assets page and extract them here.
Run `setup-casio-toolchain.sh`
Configure the makefile to use the toolchain.
Or,
Download the toolchain in the Assets page and extract them here.

Note 1: MacOS only.
Note 2: Please reserve a lot of time for running the setup toolchain, as it may take from 1.5 hours to 4 hours for the first run.

## Info
Original repo: https://git.planet-casio.com/CalcLoverHK/mpm
