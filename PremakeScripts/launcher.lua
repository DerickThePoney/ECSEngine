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
   pchheader (srcfiles.."stdafx.h")
   filter {"action:vs*", "options:not clang"}
      pchheader ("stdafx.h")
   pchsource(srcfiles.."stdafx.cpp")

   links { "ECSGameplay_Common", "ImGuiTools"}

   postbuildcommands {"{COPY} ../External/glfw-3.3.bin.WIN64/lib-vc2019/*.dll %{cfg.targetdir}"}

   includedirs { "../SRC", "../External/imgui"}

   dofile("projectsconfigs.lua")
