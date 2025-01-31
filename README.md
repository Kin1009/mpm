# MPM: Add-in loader for Math+ programming

This repo contains the sources for `mpm.bin`, the main program in the Math+ Mod (MPM) which finds add-ins, displays the add-in menu, and loads the programs.

Currently compatible with Math+ OS 2.00.

## How to use

TODO: Photos and explanations on how to use.

## How to build

Compiles with the fxSDK (just for the toolchain, this doesn't use gint).

```bash
% fxsdk build-cg # Compile
% fxlink -sw build-cg/mpm.bin # Send to calculator
```
