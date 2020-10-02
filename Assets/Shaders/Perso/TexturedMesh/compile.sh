#!/bin/bash
../../../../External/BGFX/bgfx/.build/win64_vs2019/bin/shadercRelease.exe --varyingdef varying.def.sc -f vs_TexturedMesh.sc -o vs_TexturedMesh.bin -p vs_5_0 -i ../../../../External/BGFX/bgfx/src/ --type vertex --platform windows -O 3 -V

../../../../External/BGFX/bgfx/.build/win64_vs2019/bin/shadercRelease.exe --varyingdef varying.def.sc -f fs_TexturedMesh.sc -o fs_TexturedMesh.bin -p ps_5_0 -i ../../../../External/BGFX/bgfx/src/ --type fragment --platform windows -O 3 -V