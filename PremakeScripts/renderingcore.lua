-- ecsgameplay_base.lua

project "RenderingCore"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "on"
   kind "StaticLib"
   local srcfiles = "../SRC/RenderingCore/"
   local srcfilesImGUi = "../External/imgui/imgui/"

   vpaths { 
      ["Headers"] = {srcfiles.."*.h", srcfiles.."*.inl"}, 
      ["Sources"] = {srcfiles.."*.cpp"}
      }

   files{srcfiles.."*.cpp", srcfiles.."*.h", srcfiles.."*.inl"}
   pchheader "stdafx.h"
   pchsource(srcfiles.."stdafx.cpp")

   libdirs {"../External/glfw-3.3.bin.WIN64/lib-vc2019", "../External/BGFX/bgfx/.build/win64_vs2019/bin"}
   links { "glfw3dll"}

   includedirs { "../External/imgui", "../External/glfw-3.3.bin.WIN64/include", "../External/BGFX/bgfx/include", "../External/BGFX/bimg/include", "../External/BGFX/bx/include", "../External/BGFX/bx/include/compat/msvc" }
   includedirs { "../SRC"}

   dofile("projectsconfigs.lua")

   filter "configurations:Debug"
     postbuildcommands {"{COPY} ../External/BGFX/bgfx/.build/win64_vs2019/bin/bgfxRelease.pdb %{cfg.targetdir}", "{COPY} ../External/BGFX/bgfx/.build/win64_vs2019/bin/bgfxDebug.pdb %{cfg.targetdir}"}
     links {"bgfxDebug", "bimgDebug","bxDebug", "Imgui"}
     debugenvs {"PATH=%PATH%;../External/BGFX/bgfx/.build/win64_vs2019/bin"}

   filter "configurations:not Debug"
     postbuildcommands {"{COPY} ../External/BGFX/bgfx/.build/win64_vs2019/bin/bxDebug.pdb %{cfg.targetdir}", "{COPY} ../External/BGFX/bgfx/.build/win64_vs2019/bin/bxRelease.pdb %{cfg.targetdir}"}
     links {"bgfxRelease", "bimgRelease","bxRelease", "Imgui"}
     debugenvs {"PATH=%PATH%;../External/BGFX/bgfx/.build/win64_vs2019/bin"}
