-- ecsgameplay_base.lua

project "Rendering"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "on"
   kind "StaticLib"
   local srcfiles = "../SRC/Rendering/"

   vpaths { 
      ["Headers"] = {srcfiles.."*.h", srcfiles.."*.inl"}, 
      ["Sources"] = {srcfiles.."*.cpp"},
      }
   files{srcfiles.."*.cpp", srcfiles.."*.h", srcfiles.."*.inl"}

   pchheader "stdafx.h"
   pchsource(srcfiles.."stdafx.cpp")

   libdirs {"../External/glfw-3.3.bin.WIN64/lib-vc2019", "../External/BGFX/bgfx/.build/win64_vs2019/bin"}
   links { "glfw3dll", "RenderingCore"}

   includedirs { "../External/imgui", "../External/glfw-3.3.bin.WIN64/include", "../External/BGFX/bgfx/include", "../External/BGFX/bimg/include", "../External/BGFX/bx/include", "../External/BGFX/bx/include/compat/msvc" }
   includedirs { "../SRC"}

   dofile("projectsconfigs.lua")

   filter "configurations:*"
      postbuildcommands {"{COPY} ../External/BGFX/bgfx/.build/win64_vs2019/bin/*.pdb %{cfg.targetdir}"}

   filter "configurations:Debug"
     links {"bgfxDebug", "bimgDebug","bxDebug"}
     debugenvs {"PATH=%PATH%;../External/BGFX/bgfx/.build/win64_vs2019/bin"}

   filter "configurations:not Debug"
     links {"bgfxRelease", "bimgRelease","bxRelease"}
     debugenvs {"PATH=%PATH%;../External/BGFX/bgfx/.build/win64_vs2019/bin"}
