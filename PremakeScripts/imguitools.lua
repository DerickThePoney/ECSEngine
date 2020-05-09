-- imguitools.lua

project "ImGuiTools"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "on"
   kind "StaticLib"
   local srcfiles = "../SRC/ImGuiTools/"

   vpaths { 
      ["Headers"] = {srcfiles.."*.h", srcfiles.."*.inl"}, 
      ["Sources"] = {srcfiles.."*.cpp"}
      }
   files{srcfiles.."*.cpp", srcfiles.."*.h", srcfiles.."*.inl"}

   pchheader (srcfiles.."stdafx.h")
   filter {"action:vs*", "options:not clang"}
      pchheader ("stdafx.h")
   pchsource(srcfiles.."stdafx.cpp")

   links { "Imgui", "RenderingCore" }

   includedirs { "../SRC", "../External/imgui", "../External/BGFX/bgfx/include", "../External/BGFX/bimg/include", "../External/BGFX/bx/include", "../External/BGFX/bx/include/compat/msvc" }

   dofile("projectsconfigs.lua")
