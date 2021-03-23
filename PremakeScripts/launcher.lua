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
   
   disablewarnings { "" }
   
   linkoptions{ "/WHOLEARCHIVE:application.lib", "/WHOLEARCHIVE:rendering.lib", "/WHOLEARCHIVE:renderingcore.lib", "/WHOLEARCHIVE:ecsgameplay_common.lib", "/WHOLEARCHIVE:ecsgameplay_specific.lib","/WHOLEARCHIVE:ecscore.lib", "/WHOLEARCHIVE:common.lib","/FORCE:MULTIPLE", "/IGNORE:4006", "/IGNORE:4088" }

   dofile("projectsconfigs.lua")
   
   
