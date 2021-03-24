-- launcher.lua

project "Launcher"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "on"
   kind "ConsoleApp"
   local srcfiles = "../SRC/Launcher/"

   vpaths {
      ["Headers"] = {srcfiles.."*.h", srcfiles.."*.inl"},
      ["Sources"] = {srcfiles.."*.cpp"},
      ["Debug"] = {srcfiles.."*.natvis"}
      }
   files{srcfiles.."*.cpp", srcfiles.."*.h", srcfiles.."*.inl", srcfiles.. "*.natvis"}
   pchheader (srcfiles.."stdafx.h")
   filter {"action:vs*", "options:not clang"}
      pchheader ("stdafx.h")
   pchsource(srcfiles.."stdafx.cpp")

   links { "ECSGameplay_Specific"}

   postbuildcommands {"{COPY} ../External/glfw-3.3.bin.WIN64/lib-vc2019/*.dll %{cfg.targetdir}"}

   includedirs { "../SRC"}

   dofile("projectsconfigs.lua")
