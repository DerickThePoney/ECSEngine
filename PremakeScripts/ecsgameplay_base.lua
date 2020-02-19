-- ecsgameplay_base.lua

project "ECSGameplay_Base"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "on"
   kind "StaticLib"
   local srcfiles = "../SRC/ECSGameplay_Base/"

   vpaths { 
      ["Headers"] = {srcfiles.."*.h", srcfiles.."*.inl"}, 
      ["Sources"] = {srcfiles.."*.cpp"}
      }
   files{srcfiles.."*.cpp", srcfiles.."*.h", srcfiles.."*.inl"}

   pchheader "stdafx.h"
   pchsource(srcfiles.."stdafx.cpp")

   links { "Common", "ECSBase"}

   includedirs { "../SRC"}

   dofile("projectsconfigs.lua")
