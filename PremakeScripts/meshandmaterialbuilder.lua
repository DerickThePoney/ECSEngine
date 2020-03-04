-- MeshAndMaterialBuilder.lua

project "MeshAndMaterialBuilder"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "on"
   kind "ConsoleApp"
   local srcfiles = "../SRC/Tools/MeshAndMaterialBuilder/"

   vpaths { 
      ["Headers"] = {srcfiles.."*.h", srcfiles.."*.inl"}, 
      ["Sources"] = {srcfiles.."*.cpp"},
      }
   files{srcfiles.."*.cpp", srcfiles.."*.h", srcfiles.."*.inl"}

   pchheader "stdafx.h"
   pchsource(srcfiles.."stdafx.cpp")

   links { "Application", "Rendering" }

   postbuildcommands {"{COPY} ../External/glfw-3.3.bin.WIN64/lib-vc2019/*.dll %{cfg.targetdir}"}
   
   includedirs { "../External/assimp/include", "../External/imgui"}
   includedirs { "../SRC"}

   dofile("projectsconfigs.lua")

   filter "configurations:Debug"
     postbuildcommands {"{COPY} ../External/assimp/BUILD/code/Debug/*.dll %{cfg.targetdir}"}
     postbuildcommands {"{COPY} ../External/assimp/BUILD/code/Debug/*.pdb %{cfg.targetdir}"}
     libdirs {"../External/assimp/BUILD/code/Debug"}
     links {"assimp-vc142-mtd"}

   filter "configurations:not Debug"
     postbuildcommands {"{COPY} ../External/assimp/BUILD/code/Release/*.dll %{cfg.targetdir}"}
     libdirs {"../External/assimp/BUILD/code/Release"}
     links {"assimp-vc142-mt"}
