-- ecsgameplay_base.lua

project "ECSGameplay_Specific"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "on"
   kind "StaticLib"
   local srcfiles = "../SRC/ECSGameplay_Specific/"

   vpaths {
      ["Headers"] = {srcfiles.."*.h", srcfiles.."*.inl"},
      ["Sources"] = {srcfiles.."*.cpp"}
      }
   files{srcfiles.."*.cpp", srcfiles.."*.h", srcfiles.."*.inl"}

   pchheader (srcfiles.."stdafx.h")
   filter {"action:vs*", "options:not clang"}
      pchheader ("stdafx.h")
   pchsource(srcfiles.."stdafx.cpp")

   links { "ECSGameplay_Common", "Rendering", "ImGuiTools"}

   includedirs { "../SRC"}

   dofile("projectsconfigs.lua")
