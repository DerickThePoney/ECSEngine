#!/bin/bash
../../../../External/BGFX/bgfx/.build/win64_vs2019/bin/shadercRelease.exe --varyingdef varying.def.sc -f vs_ocornut_imgui.sc -o vs_ocornut_imgui.bin -p vs_5_0 -i ../../../../External/BGFX/bgfx/src/ --type vertex --platform windows -O 3 -V
../../../../External/BGFX/bgfx/.build/win64_vs2019/bin/shadercRelease.exe --varyingdef varying.def.sc -f vs_imgui_image.sc -o vs_imgui_image.bin -p vs_5_0 -i ../../../../External/BGFX/bgfx/src/ --type vertex --platform windows -O 3 -V


../../../../External/BGFX/bgfx/.build/win64_vs2019/bin/shadercRelease.exe --varyingdef varying.def.sc -f fs_ocornut_imgui.sc -o fs_ocornut_imgui.bin -p ps_5_0 -i ../../../../External/BGFX/bgfx/src/ --type fragment --platform windows -O 3 -V
../../../../External/BGFX/bgfx/.build/win64_vs2019/bin/shadercRelease.exe --varyingdef varying.def.sc -f fs_imgui_image.sc -o fs_imgui_image.bin -p ps_5_0 -i ../../../../External/BGFX/bgfx/src/ --type fragment --platform windows -O 3 -V