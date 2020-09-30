-- common.lua

project "Common"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   kind "StaticLib"
   staticruntime "on"

   local srcfiles = "../SRC/Common/"

   vpaths {
      ["Headers"] = {srcfiles.."*.h", srcfiles.."*.inl"},
      ["Sources"] = {srcfiles.."*.cpp"}
      }
   files{srcfiles.."*.cpp", srcfiles.."*.h", srcfiles.."*.inl"}
   pchheader (srcfiles.."stdafx.h")
   filter {"action:vs*", "options:not clang"}
      pchheader ("stdafx.h")
   pchsource(srcfiles.."stdafx.cpp")

   dofile("projectsconfigs.lua")
