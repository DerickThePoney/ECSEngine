-- projectsconfigs.lua
objdir ("../obj/%{cfg.platform}/%{cfg.buildcfg}")
targetdir ("../bin/%{cfg.platform}/%{cfg.buildcfg}")
symbolspath '$(OutDir)$(TargetName).pdb'

includedirs { "../External/glm", "%BOOST_ROOT%" }
includedirs { "../External/brigand/brigand/include", "../External/cereal/include"}

filter "options:clang"
  toolset("msc-clangcl")

flags {"MultiProcessorCompile", "LinkTimeOptimization", "NoIncrementalLink"}
editAndContinue "Off"

filter "configurations:Debug"
  defines { "DEBUG" , "PERFORM_SECURITY_CHECKS"}
  symbols "On"

filter "configurations:Release"
  defines { "NDEBUG" , "PERFORM_SECURITY_CHECKS"}
  optimize "On"
  symbols "On"

filter "configurations:Profile"
  defines { "NDEBUG" , "ABSOLUTELY_NOT_ASSERT", "ENABLE_BGFX_PROFILING"}
  optimize "Speed"
  symbols "On"

filter "configurations:Final"
  defines { "NDEBUG" , "ABSOLUTELY_NOT_ASSERT"}
  optimize "Speed"
  symbols "On"
  disablewarnings{"4390"}
