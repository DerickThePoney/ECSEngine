#!/usr/bin/env python3
"""Stamp a new prototype executable and regenerate the shared prototype wiring."""

import argparse
import json
import os
import re
import subprocess
import sys

MANIFEST_PATH = "Script/Generation/prototypes.json"
RESERVED_NAMES = {
    "Launcher",
    "AssetCooker",
    "DataPacker",
    "UnitTests",
    "Common",
    "Math",
    "Application",
    "ECSCore",
    "ECSGameplay_Common",
    "Rendering",
    "RenderingCore",
    "Physics",
    "imgui",
    "ImguiTools",
    "UICore",
    "SoundCore",
    "Tracy",
    "DataPack",
    "MiniaudioSoundEngine",
}

LOADER_H = """#pragma once
#include "Common/ILoader.h"

namespace ECSEngine
{{
class {name}Loader : public ILoader
{{
public:
    {name}Loader()
        : ILoader()
    {{
    }}

    template<class Archive>
    void save(Archive& ar) const
    {{
        ar(cereal::base_class<ILoader>(this));
    }}

    template<class Archive>
    void load(Archive& ar)
    {{
        ar(cereal::base_class<ILoader>(this));
    }}

protected:
    bool VirtualInitialise() override;
    void VirtualShutdown() override;
}};
}} // namespace ECSEngine
"""

LOADER_CPP = """#include "stdafx.h"

#include "{name}Loader.h"

#include "ECSGameplay_Common/ScenarioFactory.h"
#include "{name}ScenarioUpdater.h"

CEREAL_REGISTER_TYPE(ECSEngine::{name}Loader);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ILoader, ECSEngine::{name}Loader);

namespace ECSEngine
{{
namespace
{{
IScenarioUpdater* Create{name}Scenario(const std::string& scenarioFile, bool forceReload)
{{
    {name}ScenarioUpdater* scene = new {name}ScenarioUpdater();
    scene->SetScenario(scenarioFile, forceReload);
    return scene;
}}
}}

bool {name}Loader::VirtualInitialise()
{{
    ILoader::VirtualInitialise();
    SetScenarioFactory(&Create{name}Scenario);
    return true;
}}

void {name}Loader::VirtualShutdown()
{{
    ILoader::VirtualShutdown();
}}
}} // namespace ECSEngine
"""

UPDATER_H = """#pragma once
#include "ECSGameplay_Common/CameraMoverSystem.h"
#include "ECSGameplay_Common/IScenarioUpdater.h"
#include "ECSGameplay_Common/PhysicsUpdateSystem.h"
#include "ECSGameplay_Common/SynchroWithRenderSystem.h"

namespace ECSEngine
{{
class SceneScenario;

class {name}ScenarioUpdater : public IScenarioUpdater
{{
public:
    {name}ScenarioUpdater();
    ~{name}ScenarioUpdater() override;

    void Initialise() override;
    void Destroy() override;
    void RealtimeUpdate() override;
    void GameplayUpdate() override;
    void UIUpdate() override;
    void DebugRender() override;
    void Render() override;
    void EndUpdate() override;

    void SetScenario(const std::string& parScenarioFile, bool bForceReload = false);

private:
    SceneScenario* FScenario = nullptr;
    std::string FScenarioFileName;

    CameraMoverSystem FCameraMoverSystem;
    SynchroWithRenderSystem FRenderingSystem;
    PhysicsUpdateSystem FPhysicsSystem;
}};
}} // namespace ECSEngine
"""

UPDATER_CPP = """#include "stdafx.h"

#include "{name}ScenarioUpdater.h"

#include "Application/SceneScenario.h"
#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"
#include "Common/TimeManager.h"
#include "ECSCore/WorldManager.h"
#include "ECSGameplay_Common/PhysicsJointSpawner.h"
#include "Rendering/FinalCombinePass.h"
#include "Rendering/GameRenderer.h"
#include "RenderingCore/GFXRepresentationManager.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/ImguiRenderer.h"
#include "UICore/RMLUIManager.h"

namespace ECSEngine
{{
{name}ScenarioUpdater::{name}ScenarioUpdater() = default;

{name}ScenarioUpdater::~{name}ScenarioUpdater()
{{
    delete FScenario;
}}

void {name}ScenarioUpdater::SetScenario(const std::string& parScenarioFile, bool bForceReload)
{{
    FScenarioFileName = parScenarioFile;
    AssertRelease(FScenario == nullptr);
    Resource r(parScenarioFile);
    auto handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&r);
    if (bForceReload)
    {{
        GlobalResourceCache::Instance().FCache->ForceReleaseResource(handle);
        handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&r);
    }}

    ResourceBuffer buff = handle->GetResourceBuffer();
    std::istream istr(&buff, std::istream::in);
    cereal::JSONInputArchive ar(istr);
    FScenario = new SceneScenario();
    ar(*FScenario);
}}

void {name}ScenarioUpdater::Initialise()
{{
    Rendering::GameRenderer::CreateIFP();
    Rendering::GameRenderer::Instance().Initialise();
    UI::RmlUiManager::CreateIFP();
    UI::RmlUiManager::Instance().Initialise();
    Rendering::FinalCombinePass::CreateIFP();
    Rendering::FinalCombinePass::Instance().Initialise();
    AssertRelease(FScenario != nullptr);
    FCameraMoverSystem.Init();
    FRenderingSystem.Init();
    FPhysicsSystem.Init();
    PhysicsJointSpawner::CreateIFP();
    FScenario->Initialise();
}}

void {name}ScenarioUpdater::Destroy()
{{
    WorldManager::Instance().DestroyAllRemainingEntities();
    if (FScenario != nullptr)
    {{
        FScenario->Destroy();
        delete FScenario;
        FScenario = nullptr;
    }}
    if (PhysicsJointSpawner::HasInstance())
    {{
        PhysicsJointSpawner::Instance().Finalize();
        PhysicsJointSpawner::Delete();
    }}
    FPhysicsSystem.Destroy();
    FRenderingSystem.Destroy();
    FCameraMoverSystem.Destroy();
    Rendering::FinalCombinePass::Instance().Shutdown();
    Rendering::FinalCombinePass::Destroy();
    UI::RmlUiManager::Instance().Shutdown();
    UI::RmlUiManager::Destroy();
    Rendering::GameRenderer::Instance().Shutdown();
    Rendering::GameRenderer::Destroy();
}}

void {name}ScenarioUpdater::RealtimeUpdate()
{{
    FCameraMoverSystem.Update();
}}

void {name}ScenarioUpdater::GameplayUpdate()
{{
    AssertRelease(FScenario != nullptr);
    FScenario->Update();
    FPhysicsSystem.Update();
    WorldManager::Instance().ProcessDestroyEntities();
    WorldManager::Instance().ProcessWithEntitiesCreation();
    if (PhysicsJointSpawner::HasInstance())
        PhysicsJointSpawner::Instance().Update();
    FRenderingSystem.Update();
}}

void {name}ScenarioUpdater::UIUpdate()
{{
    Rendering::ImGUI::SetImGuiContext(Rendering::RenderPassId::IMGUI_UI_PASS);
    UI::RmlUiManager::Instance().Update();
}}

void {name}ScenarioUpdater::DebugRender()
{{
}}

void {name}ScenarioUpdater::Render()
{{
    Rendering::GFXRepresentationManager::Instance().OnGameplayFrameEnded();
    Rendering::GFXRepresentationManager::Instance().Update(TimeManager::FrameStartTime());
    Rendering::GameRenderer::Instance().Render();
    UI::RmlUiManager::Instance().Render();
    Rendering::FinalCombinePass::Instance().SetTextures(Rendering::GameRenderer::Instance().GetFinalTexture(), UI::RmlUiManager::Instance().GetTexture());
    Rendering::FinalCombinePass::Instance().Render();
}}

void {name}ScenarioUpdater::EndUpdate()
{{
}}
}} // namespace ECSEngine
"""

IDENTITY_CPP = """#include "stdafx.h"

#include "Application/BaseApplication.h"
#include "Application/CommonLoaders.h"
#include "ECSGameplay_Common/ECSLoader.h"
#include "Games/{name}/{name}Loader.h"
#include "Launcher/ApplicationUpdater.h"
#include "Launcher/GameIdentity.h"
#include "Physics/PhysicsLoader.h"
#include "RenderingCore/RenderingLoader.h"
#include "SoundCore/SoundLoader.h"

namespace ECSEngine
{{
const char* GetGameAssetsDirectory()
{{
    return "..\\\\Assets\\\\{name}";
}}

void InstallDefaultApplication(BaseApplicationLayer& app)
{{
    app.AddNewLoader<LoaderInitialiseCommonResources>();
    app.AddNewLoader<ECSLoader>("\\\\Configuration\\\\EntityTemplates.json");
    app.AddNewLoader<SoundLoader>();
    app.AddNewLoader<PhysicsLoader>();
    app.AddNewLoader<RenderingLoader>("{name}");
    app.AddNewLoader<{name}Loader>();
    app.SetGameplayUpdater_StealOwnership(new ApplicationUpdaterWrapper());
}}
}} // namespace ECSEngine
"""

STDAFX_H = """#pragma once

#include "Application/stdafx.h"
#include "imgui/imgui.h"
"""

STDAFX_CPP = """#include "stdafx.h"
"""

BASE_APPLICATION = """{{
    "value0": {{
        "value0": [
            {{
                "polymorphic_id": 2147483649,
                "polymorphic_name": "ECSEngine::LoaderInitialiseCommonResources",
                "ptr_wrapper": {{
                    "valid": 1,
                    "data": {{
                        "value0": {{
                            "LoaderName": "ECSEngine::LoaderInitialiseCommonResources"
                        }},
                        "SceneManagerConfigFile": "Configuration\\\\SceneManager.json"
                    }}
                }}
            }},
            {{
                "polymorphic_id": 2147483654,
                "polymorphic_name": "ECSEngine::SoundLoader",
                "ptr_wrapper": {{
                    "valid": 1,
                    "data": {{
                        "value0": {{
                            "LoaderName": "ECSEngine::SoundLoader"
                        }}
                    }}
                }}
            }},
            {{
                "polymorphic_id": 2147483652,
                "polymorphic_name": "ECSEngine::PhysicsLoader",
                "ptr_wrapper": {{
                    "valid": 1,
                    "data": {{
                        "value0": {{
                            "LoaderName": "ECSEngine::PhysicsLoader"
                        }},
                        "ConfigurationFilename": "Configuration\\\\PhysicsConfiguration.json"
                    }}
                }}
            }},
            {{
                "polymorphic_id": 2147483651,
                "polymorphic_name": "ECSEngine::RenderingLoader",
                "ptr_wrapper": {{
                    "valid": 1,
                    "data": {{
                        "value0": {{
                            "LoaderName": "ECSEngine::RenderingLoader"
                        }},
                        "ApplicationName": "{name}",
                        "RepresentationDescriptors": "Configuration\\\\GFXRepresentationDescriptors.json"
                    }}
                }}
            }},
            {{
                "polymorphic_id": 2147483650,
                "polymorphic_name": "ECSEngine::ECSLoader",
                "ptr_wrapper": {{
                    "valid": 1,
                    "data": {{
                        "value0": {{
                            "LoaderName": "ECSEngine::ECSLoader"
                        }},
                        "EntityTemplatesFile": "Configuration\\\\EntityTemplates.json",
                        "FontsFile": "Configuration\\\\FontsConfiguration.json"
                    }}
                }}
            }},
            {{
                "polymorphic_id": 2147483655,
                "polymorphic_name": "ECSEngine::{name}Loader",
                "ptr_wrapper": {{
                    "valid": 1,
                    "data": {{
                        "value0": {{
                            "LoaderName": "ECSEngine::{name}Loader"
                        }}
                    }}
                }}
            }}
        ],
        "value1": {{
            "polymorphic_id": 2147483653,
            "polymorphic_name": "ECSEngine::ApplicationUpdaterWrapper",
            "ptr_wrapper": {{
                "valid": 1,
                "data": {{}}
            }}
        }}
    }}
}}
"""

SCENE_MANAGER = """{{
    "SceneManager": {{
        "Scenes": [
            "scenes\\\\empty.scene"
        ]
    }}
}}
"""

ENTITY_TEMPLATES = """{{
    "EntityTemplatesList": {{
        "EntityTemplates": []
    }}
}}
"""


def load_manifest():
    if not os.path.isfile(MANIFEST_PATH):
        return []
    with open(MANIFEST_PATH, "r", encoding="utf-8") as manifest_file:
        return json.load(manifest_file)


def save_manifest(prototypes):
    with open(MANIFEST_PATH, "w", encoding="utf-8", newline="\n") as manifest_file:
        json.dump(prototypes, manifest_file, indent=2)
        manifest_file.write("\n")


def write_if_absent(path, contents):
    if os.path.exists(path):
        return
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w", encoding="utf-8", newline="\n") as output:
        output.write(contents)


def bff_string_list(names):
    return "{ %s }" % ", ".join("'%s'" % name for name in names)


def project_block(project_name, exe_name, build_target, solution_folder):
    return """
{{
    .ProjectName = '{project}'
    .ProjectPath = 'SRC/Games'
    .AdditionnalIncludePaths = {{ 'SRC' }}
    .SolutionFolder = '{folder}'
    .ExeName = '{exe}'
    #define USE_PRECOMPILED_HEADERS
    .BuildTarget = {{ '{target}' }}
    #include "../Utils/MakeProjectStandard.bff"
    #undef USE_PRECOMPILED_HEADERS
}}
""".format(project=project_name, exe=exe_name, target=build_target, folder=solution_folder)


def regenerate(prototypes):
    os.makedirs("BFF/Generated", exist_ok=True)
    os.makedirs("SRC/Games/Generated", exist_ok=True)

    libraries = [item["name"] for item in prototypes]
    launchers = ["%sLauncher" % item["name"] for item in prototypes]
    extra_exes = []
    for item in prototypes:
        exe = item.get("exe", item["name"])
        if exe != "BuildingGame" and exe not in extra_exes:
            extra_exes.append(exe)

    parts = [
        "#once\n",
        ".AllPrototypeLibraries = %s\n" % bff_string_list(libraries),
        ".AllPrototypeLauncherProjects = %s\n" % bff_string_list(launchers),
        ".PrototypeExecutables = %s\n" % bff_string_list(extra_exes),
    ]
    for exe in extra_exes:
        parts.append(".%sProjects = {}\n" % exe)
    for item in prototypes:
        name = item["name"]
        exe = item.get("exe", name)
        parts.append(project_block(name, exe, "$ProjectName$", "Gameplay"))
        parts.append(project_block("%sLauncher" % name, exe, "$ExeName$-Exe", "GameLaunchers"))
        if exe != "BuildingGame":
            parts.append(
                """
{{
    .ExeName = '{exe}'
    .ProjectList = .EngineProjects + {{ '{name}', '{name}Launcher' }}
    #include "../Utils/Executable.bff"
}}
""".format(exe=exe, name=name)
            )

    with open("BFF/Generated/Prototypes.bff", "w", encoding="utf-8", newline="\n") as bff:
        bff.write("".join(parts))

    generated = {
        "SRC/Games/Generated/ModuleLists.inl": "ModuleList.inl",
        "SRC/Games/Generated/MousePolicies.inl": "MousePolicyType.inl",
        "SRC/Games/Generated/MessageIds.inl": "GenericMessageIdentifiers.inl",
        "SRC/Games/Generated/SceneActions.inl": "SceneActionIds.inl",
    }
    for path, filename in generated.items():
        lines = ['#include "Games/%s/%s"\n' % (item["name"], filename) for item in prototypes]
        with open(path, "w", encoding="utf-8", newline="\n") as output:
            output.writelines(lines)


def stamp_sources(name):
    root = "SRC/Games/%s" % name
    launcher = "SRC/Games/%sLauncher" % name
    assets = "Assets/%s" % name
    fields = {"name": name}
    write_if_absent(os.path.join(root, "stdafx.h"), STDAFX_H)
    write_if_absent(os.path.join(root, "stdafx.cpp"), STDAFX_CPP)
    write_if_absent(os.path.join(root, "ModuleList.inl"), "")
    write_if_absent(os.path.join(root, "MousePolicyType.inl"), "")
    write_if_absent(os.path.join(root, "GenericMessageIdentifiers.inl"), "")
    write_if_absent(os.path.join(root, "SceneActionIds.inl"), "")
    write_if_absent(os.path.join(root, "%sLoader.h" % name), LOADER_H.format(**fields))
    write_if_absent(os.path.join(root, "%sLoader.cpp" % name), LOADER_CPP.format(**fields))
    write_if_absent(os.path.join(root, "%sScenarioUpdater.h" % name), UPDATER_H.format(**fields))
    write_if_absent(os.path.join(root, "%sScenarioUpdater.cpp" % name), UPDATER_CPP.format(**fields))
    write_if_absent(os.path.join(launcher, "stdafx.h"), "#pragma once\n\n#include \"Application/stdafx.h\"\n")
    write_if_absent(os.path.join(launcher, "stdafx.cpp"), STDAFX_CPP)
    write_if_absent(os.path.join(launcher, "%sLauncher.cpp" % name), IDENTITY_CPP.format(**fields))

    scene_template = "Script/Generation/templates/Empty.scene"
    write_if_absent(os.path.join(assets, "Configuration", "BaseApplication.json"), BASE_APPLICATION.format(**fields))
    write_if_absent(os.path.join(assets, "Configuration", "SceneManager.json"), SCENE_MANAGER)
    write_if_absent(os.path.join(assets, "Configuration", "EntityTemplates.json"), ENTITY_TEMPLATES)
    if os.path.isfile(scene_template):
        with open(scene_template, "r", encoding="utf-8") as scene_file:
            scene = scene_file.read().replace("Empty", name)
        write_if_absent(os.path.join(assets, "Scenes", "Empty.scene"), scene)


def valid_name(name):
    return re.match(r"^[A-Z][A-Za-z0-9]*$", name) is not None


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--name", required=True)
    parser.add_argument("--exe", default=None)
    parser.add_argument("--register-only", action="store_true")
    args = parser.parse_args()

    name = args.name
    if not valid_name(name) or name in RESERVED_NAMES:
        print("Prototype name must be a PascalCase identifier and must not clash with an engine project")
        return 1

    exe = args.exe if args.exe else name
    prototypes = load_manifest()
    if any(item["name"] == name for item in prototypes):
        print("%s is already in %s" % (name, MANIFEST_PATH))
        return 1

    source_dir = "SRC/Games/%s" % name
    launcher_dir = "SRC/Games/%sLauncher" % name
    if args.register_only:
        if not os.path.isdir(source_dir) or not os.path.isdir(launcher_dir):
            print("--register-only requires %s and %s" % (source_dir, launcher_dir))
            return 1
    elif os.path.exists(source_dir) or os.path.exists(launcher_dir):
        print("%s already exists. Use --register-only to wire it without stamping sources." % name)
        return 1
    else:
        stamp_sources(name)

    prototypes.append({"name": name, "exe": exe})
    save_manifest(prototypes)
    regenerate(prototypes)

    result = subprocess.run(["py", "-u", "Script/FastBuild/fastbuild.py"])
    return result.returncode


if __name__ == "__main__":
    sys.exit(main())
