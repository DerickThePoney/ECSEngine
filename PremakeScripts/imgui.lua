-- common.lua

project "Imgui"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   kind "StaticLib"
   staticruntime "on"

   local srcfiles = "../External/imgui/imgui/"

   vpaths { 
      ["Headers"] = {srcfiles.."*.h", srcfiles.."*.inl"}, 
      ["Sources"] = {srcfiles.."*.cpp"}
      }
   files{srcfiles.."*.cpp", srcfiles.."*.h"}

   dofile("projectsconfigs.lua")