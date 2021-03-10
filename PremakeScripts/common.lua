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
   files{srcfiles.."*.cpp", srcfiles.."*.h", srcfiles.."*.inl", "../External/Remotery/lib/Remotery.c", "../External/Remotery/lib/Remotery.h"}
   pchheader (srcfiles.."stdafx.h")
   filter {"action:vs*", "options:not clang"}
      pchheader ("stdafx.h")
   pchsource(srcfiles.."stdafx.cpp")

   includedirs { "../SRC", "../External/polypartition-master/src"}
   links("FMT")

   dofile("projectsconfigs.lua")

   filter { 'files:../External/Remotery/lib/Remotery.c' }
      flags { 'NoPCH' }