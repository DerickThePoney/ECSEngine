-- launcher.lua

project "Launcher"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "on"
   kind "ConsoleApp"
   local srcfiles = "../SRC/Launcher/"

   vpaths { 
      ["Headers"] = {srcfiles.."*.h", srcfiles.."*.inl"}, 
      ["Sources"] = {srcfiles.."*.cpp"}
      }
   files{srcfiles.."*.cpp", srcfiles.."*.h", srcfiles.."*.inl"}
   pchheader "stdafx.h"
   pchsource(srcfiles.."stdafx.cpp")

   links { "Common", "ECSBase", "ECSGameplay_Base", "RenderingBase"}

   postbuildcommands {"{COPY} ../External/glfw-3.3.bin.WIN64/lib-vc2019/*.dll %{cfg.targetdir}"}

   includedirs { "../SRC"}

   dofile("projectsconfigs.lua")
