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
   pchsource(srcfiles.."stdafx.cpp")

   includedirs { "../External/assimp/include","../External/assimp/BUILD/include"}
   includedirs { "../SRC"}

   dofile("projectsconfigs.lua")

   filter "configurations:Debug"
     postbuildcommands {"{COPY} ../External/assimp/BUILD/code/Debug/*.dll %{cfg.targetdir}"}
     postbuildcommands {"{COPY} ../External/assimp/BUILD/code/Debug/*.pdb %{cfg.targetdir}"}
     libdirs {"../External/assimp/BUILD/code/Debug"}
     links {"assimp-vc142-mtd"} -- , "IrrXMLd", "zlibstaticd"

   filter "configurations:not Debug"
     postbuildcommands {"{COPY} ../External/assimp/BUILD/code/Release/*.dll %{cfg.targetdir}"}
     libdirs {"../External/assimp/BUILD/code/Release"}
     links {"assimp-vc142-mt"}
