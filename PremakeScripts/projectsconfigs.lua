-- projectsconfigs.lua
objdir ("../obj/%{cfg.platform}/%{cfg.buildcfg}")
targetdir ("../bin/%{cfg.platform}/%{cfg.buildcfg}")
symbolspath '$(OutDir)$(TargetName).pdb'

includedirs { "../External/glm", "D:/Applications/boost_1_70_0" }
includedirs { "../External/brigand/brigand/include"}

filter "configurations:Debug"
  defines { "DEBUG" , "PERFORM_SECURITY_CHECKS"}
  symbols "On"

filter "configurations:Release"
  defines { "NDEBUG" , "PERFORM_SECURITY_CHECKS"}
  optimize "On"
  symbols "On"

filter "configurations:Profile"
  defines { "NDEBUG" , "ABSOLUTELY_NOT_ASSERT", "ENABLE_BGFX_PROFILING"}
  optimize "On"
  symbols "On"

filter "configurations:Final"
  defines { "NDEBUG" , "ABSOLUTELY_NOT_ASSERT"}
  optimize "On"
  symbols "On"
