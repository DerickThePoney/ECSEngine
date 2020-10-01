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
   filter {"action:vs*", "options:not clang"}
      pchheader ("stdafx.h")
   pchsource(srcfiles.."stdafx.cpp")

   includedirs { "../External/assimp/include","../External/assimpBinaries/include"}
   includedirs { "../SRC"}

   links{"Application", "AssimpWrapper", "RenderingCore"}

   dofile("projectsconfigs.lua")

   filter "configurations:Debug"
     postbuildcommands {"{COPY} ../External/assimpBinaries/Debug/*.dll %{cfg.targetdir}"}
     postbuildcommands {"{COPY} ../External/assimpBinaries/Debug/*.pdb %{cfg.targetdir}"}

   filter "configurations:not Debug"
     postbuildcommands {"{COPY} ../External/assimpBinaries/Release/*.dll %{cfg.targetdir}"}
