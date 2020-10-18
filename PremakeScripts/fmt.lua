-- fmt.lua

project "FMT"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   kind "StaticLib"
   staticruntime "on"

   local srcfiles = "../External/fmt/fmt/"

   includedirs { "../External/fmt"}

   vpaths {
      ["Headers"] = {srcfiles.."*.h", srcfiles.."*.inl"},
      ["Sources"] = {srcfiles.."*.cc"}
      }
   files{srcfiles.."*.cc", srcfiles.."*.h"}
   objdir ("../obj/%{cfg.platform}/%{cfg.buildcfg}")
   targetdir ("../bin/%{cfg.platform}/%{cfg.buildcfg}")
   symbolspath '$(OutDir)$(TargetName).pdb'

   filter "options:clang"
     toolset("msc-clangcl")

   flags {"MultiProcessorCompile", "LinkTimeOptimization", "NoIncrementalLink"}
   editAndContinue "Off"

   filter "configurations:Debug"
     defines { "DEBUG" , "PERFORM_SECURITY_CHECKS", "_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING"}
     symbols "On"

   filter "configurations:Release"
     defines { "NDEBUG" , "PERFORM_SECURITY_CHECKS", "_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING"}
     optimize "On"
     symbols "On"

   filter "configurations:Profile"
     defines { "NDEBUG" , "ABSOLUTELY_NOT_ASSERT", "ENABLE_BGFX_PROFILING", "_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING"}
     optimize "Speed"
     symbols "On"

   filter "configurations:Final"
     defines { "NDEBUG" , "ABSOLUTELY_NOT_ASSERT", "_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING"}
     optimize "Speed"
     symbols "On"
     disablewarnings{"4390"}