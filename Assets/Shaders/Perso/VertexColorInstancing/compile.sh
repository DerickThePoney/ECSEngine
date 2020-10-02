#!/bin/bash
../../../../External/BGFX/bgfx/.build/win64_vs2019/bin/shadercRelease.exe --varyingdef varying.def.sc -f vs_VertexColorInstancing.sc -o vs_VertexColorInstancing.bin -p vs_5_0 -i ../../../../External/BGFX/bgfx/src/ --type vertex --platform windows -O 3 -V

../../../../External/BGFX/bgfx/.build/win64_vs2019/bin/shadercRelease.exe --varyingdef varying.def.sc -f fs_VertexColorInstancing.sc -o fs_VertexColorInstancing.bin -p ps_5_0 -i ../../../../External/BGFX/bgfx/src/ --type fragment --platform windows -O 3 -V