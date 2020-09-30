-- assimpwrapper.lua

project "AssimpWrapper"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "on"
   kind "StaticLib"
   local srcfiles = "../SRC/Tools/AssimpWrapper/"

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

   dofile("projectsconfigs.lua")

   filter "configurations:Debug"
     postbuildcommands {"{COPY} ../External/assimpBinaries/Debug/*.dll %{cfg.targetdir}"}
     postbuildcommands {"{COPY} ../External/assimpBinaries/Debug/*.pdb %{cfg.targetdir}"}
     libdirs {"../External/assimpBinaries/Debug"}
     links {"assimp-vc142-mtd"} -- , "IrrXMLd", "zlibstaticd"

   filter "configurations:not Debug"
     postbuildcommands {"{COPY} ../External/assimpBinaries/Release/*.dll %{cfg.targetdir}"}
     libdirs {"../External/assimpBinaries/Release"}
     links {"assimp-vc142-mt"}
