#include "stdafx.h"

#include "SceneManagerEditor.h"

#include "Application/PropertyDrawer.h"
#include "Application/SceneManager.h"

namespace ECSEngine
{
namespace ImGUITools
{

void DrawScenesManagerEditor(bool* parShow)
{
    std::vector<std::string>& scenes = SceneManager::Instance().ScenesForWriting();
    ImGui::Begin("Scene manager", parShow);

    static std::string sceneToAdd = "";
    EDITOR_PROPERTY_STRING("Scene To Add", sceneToAdd, true, "*.scene");

    ImGui::SameLine();
    if (!sceneToAdd.empty() && ImGui::Button("Add scene"))
    {
        if (std::find(scenes.begin(), scenes.end(), sceneToAdd) == scenes.end())
            scenes.push_back(sceneToAdd);
    }

    ImGui::SameLine();
    if (ImGui::Button("Save"))
    {
        std::ofstream ofstr(GlobalResourceCache::Instance().FCache->GetBasePath() + "\\Configuration\\SceneManager.json");

        cereal::JSONOutputArchive archive(ofstr);

        archive(NAMEDPROPERTY("SceneManager", SceneManager::Instance()));
    }

    ImGui::BeginChild("ListOfScenes", glm::vec2(-10.f, -10.f), true);
    bool up = false;
    bool down = false;
    bool suppr = false;
    u32 i = 0;
    foreachitemconst(scene, scenes)
    {
        ImGui::Text("%d - %s", (i + 1), scene.c_str());

        ImGui::SameLine();
        if (i > 0 && ImGui::Button("Up"))
        {
            up = true;
            break;
        }

        ImGui::SameLine();
        if (i < (scenes.size() - 1) && ImGui::Button("Down"))
        {
            down = true;
            break;
        }

        ImGui::SameLine();
        if (ImGui::Button("Del"))
        {
            suppr = true;
            break;
        }
        ++i;
    }

    if (up)
    {
        auto a = scenes[i];
        scenes[i] = scenes[i - 1];
        scenes[i - 1] = a;
    }

    if (down)
    {
        auto a = scenes[i];
        scenes[i] = scenes[i + 1];
        scenes[i + 1] = a;
    }

    if (suppr)
    {
        scenes.erase(scenes.begin() + i);
    }

    ImGui::EndChild();

    ImGui::End();
}

} // namespace ImGUITools
} // namespace ECSEngine
