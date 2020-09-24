#!/bin/bash
../../../../External/BGFX/bgfx/.build/win64_vs2019/bin/shadercRelease.exe --varyingdef varying.def.sc -f vs_EditorGrid.sc -o vs_EditorGrid.bin -p vs_5_0 -i ../../../../External/BGFX/bgfx/src/ --type vertex --platform windows -O 3

../../../../External/BGFX/bgfx/.build/win64_vs2019/bin/shadercRelease.exe --varyingdef varying.def.sc -f fs_EditorGrid.sc -o fs_EditorGrid.bin -p ps_5_0 -i ../../../../External/BGFX/bgfx/src/ --type fragment --platform windows -O 3 