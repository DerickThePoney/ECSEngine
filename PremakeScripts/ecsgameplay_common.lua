-- ecsgameplay_base.lua

project "ECSGameplay_Common"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "on"
   kind "StaticLib"
   local srcfiles = "../SRC/ECSGameplay_Common/"

   vpaths { 
      ["Headers"] = {srcfiles.."*.h", srcfiles.."*.inl"}, 
      ["Sources"] = {srcfiles.."*.cpp"}
      }
   files{srcfiles.."*.cpp", srcfiles.."*.h", srcfiles.."*.inl"}

   pchheader (srcfiles.."stdafx.h")
   filter {"action:vs*", "options:not clang"}
      pchheader ("stdafx.h")
   pchsource(srcfiles.."stdafx.cpp")

   links { "ECSCore", "Rendering"}

   includedirs { "../SRC", "../External/imgui"}

   dofile("projectsconfigs.lua")
