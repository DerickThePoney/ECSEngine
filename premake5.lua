-- premake5.lua
workspace "ECSEngine"
   configurations { "Debug", "Release", "Profile", "Final" }
   platforms { "Win64" }

   location ("build/")

   filter { "platforms:Win64" }
    system "Windows"
    architecture "x64"

   --filter { "platforms:Win32" }
    --system "Windows"
    --architecture "x86"

   -- Get that C++17 goodness
   cppdialect  "C++17"

   -- TODO
   -- prebuildcommands {"premake5.exe --file=..\\premake5.lua vs2019"}

group "Core"
    include("PremakeScripts/fmt.lua")
    include("PremakeScripts/common.lua")
    include("PremakeScripts/ecscore.lua")
    include("PremakeScripts/application.lua")
group "" -- end of "Dependensies"

group "Rendering"
    include("PremakeScripts/imgui.lua")
    include("PremakeScripts/renderingcore.lua")
    include("PremakeScripts/rendering.lua")
group ""


group "Gameplay"
    include("PremakeScripts/ecsgameplay_common.lua")
    include("PremakeScripts/ecsgameplay_specific.lua")
group ""

group "ImGuiTools"
    include("PremakeScripts/imguitools.lua")
group ""

group "Tools"
    include("PremakeScripts/assimpwrapper.lua")
    include("PremakeScripts/assetcooker.lua")
group ""

include("PremakeScripts/launcher.lua")