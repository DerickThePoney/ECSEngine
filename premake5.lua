-- premake5.lua
workspace "ECSEngine"
   configurations { "Debug", "Release", "Final" }
   platforms { "Win64" }

   location ("build/")

   filter { "platforms:Win64" }
    system "Windows"
    architecture "x64"

   -- Get that C++14 goodness
   cppdialect  "C++17"

   -- TODO
   -- prebuildcommands {"premake5.exe --file=..\\premake5.lua vs2019"}

include("PremakeScripts/common.lua")

include("PremakeScripts/renderingbase.lua")

include("PremakeScripts/ecsbase.lua")

include("PremakeScripts/ecsgameplay_base.lua")

include("PremakeScripts/launcher.lua")