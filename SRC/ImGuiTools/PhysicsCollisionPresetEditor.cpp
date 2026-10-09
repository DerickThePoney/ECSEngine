#include "stdafx.h"

#include "PhysicsCollisionPresetEditor.h"

#include "Physics/PhysicsAPI.h"
#include "Physics/PhysicsCollisionPresetManager.h"
#include "Physics/PhysicsEngineConfiguration.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"

#include <fstream>

namespace ECSEngine
{
namespace ImGUITools
{
class PhysicsCollisionPresetEditor
{
public:
    void DrawEditor(bool* parOpen, float parMenuBarHeight);

private:
    void DrawPhysicsCollisionPresetMenuBar();

    void DrawCollisionPresetsList();
    void DrawCollisionPresetEditor();

    u32 FSelected = -1;
};

void PhysicsCollisionPresetEditor::DrawEditor(bool* parOpen, float parMenuBarHeight)
{
    const uvec2 uwindowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    const vec2 windowSize(uwindowSize.x, uwindowSize.y);
    ImGui::SetNextWindowSize(windowSize * vec2(1.0f, 1.0f - (parMenuBarHeight / windowSize.y)));
    ImGui::SetNextWindowPos(vec2(0.0f, parMenuBarHeight));
    ImGui::Begin("Physics Collision Presets Editor", parOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);
    DrawPhysicsCollisionPresetMenuBar();

    const vec2 currentWindowSize = ImGui::GetContentRegionAvail();
    const vec2 utilityPlace = currentWindowSize - 50.0f;
    const float listProportion = 0.30f;
    const vec2 listSize = utilityPlace * vec2(listProportion, 1.0f);
    ImGui::SetCursorPosX(((currentWindowSize - utilityPlace) * 0.5f).x);
    ImGui::BeginChild(ImGui::GetID("Collision Presets list"), listSize, true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    DrawCollisionPresetsList();
    ImGui::EndChild();

    ImGui::SameLine();

    const vec2 editorPlace = utilityPlace * vec2(1 - listProportion, 1.0f);
    ImGui::BeginChild(ImGui::GetID("Collision Preset editor"), editorPlace, true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    DrawCollisionPresetEditor();
    ImGui::EndChild();

    ImGui::End();
}

void PhysicsCollisionPresetEditor::DrawPhysicsCollisionPresetMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save"))
            {
                std::ofstream ofstr(GlobalResourceCache::Instance().FCache->GetBasePath() + "\\Configuration\\PhysicsCollisionPresets.json");

                cereal::JSONOutputArchive archive(ofstr);
                const Physics::PhysicsCollisionPresetManager& Manager = Physics::GetCollisionPresetManager();
                archive(NAMEDPROPERTY("PhysicsCollisionPresets", Manager.CollisionPresets()));
            }

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void PhysicsCollisionPresetEditor::DrawCollisionPresetsList()
{
    Physics::PhysicsCollisionPresetManager& Manager = Physics::GetCollisionPresetManager();
    std::vector<Physics::PhysicsCollisionPreset>& CollisionPresets = Manager.CollisionPresets();

    if (ImGui::Button("Add new preset"))
    {
        CollisionPresets.push_back(Physics::PhysicsCollisionPreset{ });
    }
    ImGui::Separator();

    u32 ToDel = -1;
    forrange(i, 0, CollisionPresets.size())
    {
        ImGui::PushID(i);
        if (ImGui::Selectable(CollisionPresets[i].FName.c_str(), FSelected == i, ImGuiSelectableFlags_AllowOverlap))
        {
            FSelected = (u32)i;
        }
        ImGui::SameLine();
        if (ImGui::Button("Del"))
        {
            ToDel = (u32)i;
        }
        ImGui::PopID();
    }

    if (ToDel != -1)
    {
        CollisionPresets.erase(CollisionPresets.begin() + ToDel);
    }
}

void PhysicsCollisionPresetEditor::DrawCollisionPresetEditor()
{
    Physics::PhysicsCollisionPresetManager& Manager = Physics::GetCollisionPresetManager();
    const Physics::PhysicsEngineConfiguration& Config = Physics::GetConfig();
    std::vector<Physics::PhysicsCollisionPreset>& CollisionPresets = Manager.CollisionPresets();
    if (FSelected == -1 || FSelected >= CollisionPresets.size())
        return;

    Physics::PhysicsCollisionPreset& Current = CollisionPresets[FSelected];

    // Name updater
    char text[256];
    sprintf(text, "%s", Current.FName.c_str());
    ImGui::InputText("##PresetName", text, 256);
    Current.FName = std::string(text);

    // Category Updater
    std::string Category = Config.LayerName(Current.FCollisionCategory);

    if (ImGui::BeginCombo("Collision Category", Category.c_str()))
    {
        forrange(i, 0, Config.FNumLayers)
        {
            bool bIsSelected = Current.FCollisionCategory == i;
            if (ImGui::Selectable(Config.LayerName(i).c_str(), bIsSelected))
            {
                Current.FCollisionCategory = i;
            }
            if (bIsSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::Separator();

    ImGui::Text("Collision channels to collide with");

    const u32 AllLayersMask = ((1u << Config.FNumLayers) - 1);
    const bool bIsAllChannels = (Current.FCollisionMask & AllLayersMask) == AllLayersMask;

    if (bIsAllChannels)
        Current.FCollisionMask = -1u;

    bool bWantsAllChannels = bIsAllChannels;
    ImGui::Checkbox("Collide all", &bWantsAllChannels);

    if (!bIsAllChannels && bWantsAllChannels)
        Current.FCollisionMask = -1u;

    ImGui::SameLine();
    bool bWantsNoChannels = Current.FCollisionMask == 0;
    ImGui::Checkbox("Collide None", &bWantsNoChannels);
    if (bWantsNoChannels)
        Current.FCollisionMask = 0;

    static constexpr i32 NbColumns = 4;
    if (ImGui::BeginTable("CollidesWith", NbColumns))
    {
        forrange(i, 0, Config.FNumLayers)
        {
            if ((i % NbColumns) == 0)
            {
                ImGui::TableNextRow();
            }
            ImGui::TableNextColumn();
            ImGui::CheckboxFlags(Config.LayerName(i).c_str(), &Current.FCollisionMask, 1u << i);
        }
        ImGui::EndTable();
    }
}

void DrawPhysicsCollisionPresetsEditor(bool* parOpen, float parMenuBarHeigth)
{
    static PhysicsCollisionPresetEditor e;
    e.DrawEditor(parOpen, parMenuBarHeigth);
}

} // namespace ImGUITools
} // namespace ECSEngine
