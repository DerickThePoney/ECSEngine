-- ecsbase.lua

project "ECSCore"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "on"
   kind "StaticLib"
   local srcfiles = "../SRC/ECSCore/"

   vpaths { 
      ["Headers"] = {srcfiles.."*.h", srcfiles.."*.inl"}, 
      ["Sources"] = {srcfiles.."*.cpp"}
      }
   files{srcfiles.."*.cpp", srcfiles.."*.h", srcfiles.."*.inl"}
   pchheader "stdafx.h"
   pchsource(srcfiles.."stdafx.cpp")

   links { "Common" }

   includedirs { "../SRC"}

   dofile("projectsconfigs.lua")