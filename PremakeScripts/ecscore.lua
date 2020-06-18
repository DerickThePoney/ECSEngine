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
   pchheader (srcfiles.."stdafx.h")
   pchsource(srcfiles.."stdafx.cpp")

   links { "Application" }

   includedirs { "../SRC",  "../External/imgui"}

   dofile("projectsconfigs.lua")
