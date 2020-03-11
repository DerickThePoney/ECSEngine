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

   pchheader "stdafx.h"
   pchsource(srcfiles.."stdafx.cpp")

   links { "Imgui" }

   includedirs { "../SRC", "../External/imgui"}

   dofile("projectsconfigs.lua")
