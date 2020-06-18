-- AssetCooker.lua

project "AssetCooker"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "on"
   kind "ConsoleApp"
   local srcfiles = "../SRC/Tools/AssetCooker/"

   vpaths { 
      ["Headers"] = {srcfiles.."*.h", srcfiles.."*.inl"}, 
      ["Sources"] = {srcfiles.."*.cpp"},
      }
   files{srcfiles.."*.cpp", srcfiles.."*.h", srcfiles.."*.inl"}

   pchheader (srcfiles.."stdafx.h")
   pchsource(srcfiles.."stdafx.cpp")
   
   includedirs { "../External/assimp/include","../External/assimp/BUILD/include"}
   includedirs { "../SRC"}

   links{"Application", "AssimpWrapper", "RenderingCore"}

   dofile("projectsconfigs.lua")

   filter "configurations:Debug"
     postbuildcommands {"{COPY} ../External/assimp/BUILD/code/Debug/*.dll %{cfg.targetdir}"}
     postbuildcommands {"{COPY} ../External/assimp/BUILD/code/Debug/*.pdb %{cfg.targetdir}"}

   filter "configurations:not Debug"
     postbuildcommands {"{COPY} ../External/assimp/BUILD/code/Release/*.dll %{cfg.targetdir}"}
