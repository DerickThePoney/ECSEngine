#include "stdafx.h"

#include "GameRulesEditor.h"

#include "Common/ResourceCache.h"
#include "ECSGameplay_Specific/GameplayRulesManager.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"

namespace ECSEngine
{
namespace ImGUITools
{

class GameRulesEditor
{
public:
    void DrawEditor(bool* parOpen, float parMenuBarHeight);

private:
    void DrawGameRulesMenuBar();
    void DrawSpawnRulesEditor();
    void DrawBuildingCostsEditor();
    void DrawConstantsEditor();

private:
    bool isEditingSpawnRules = true;
};

void GameRulesEditor::DrawEditor(bool* parOpen, float parMenuBarHeight)
{
    glm::vec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    ImGui::SetNextWindowSize(windowSize * glm::vec2(1.0f, 1.0f - (parMenuBarHeight / windowSize.y)));
    ImGui::SetNextWindowPos(glm::vec2(0.0f, parMenuBarHeight));
    ImGui::Begin("Game rules editor", parOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);
    DrawGameRulesMenuBar();

    if (ImGui::BeginTabBar("##GameplayRulesTabBar"))
    {
        if (ImGui::BeginTabItem("Peon Spawn Rules", &isEditingSpawnRules))
        {
            DrawSpawnRulesEditor();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Gameplay constants", &isEditingSpawnRules))
        {
            DrawConstantsEditor();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Building costs rules", &isEditingSpawnRules))
        {
            DrawBuildingCostsEditor();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

void GameRulesEditor::DrawGameRulesMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save"))
            {
                std::ofstream ofstr(GlobalResourceCache::Instance().FCache->GetBasePath() + "\\Configuration\\GameplayRules.json");

                cereal::JSONOutputArchive archive(ofstr);

                archive(NAMEDPROPERTY("GameplayRules", GameplayRulesManager::Instance()));
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void GameRulesEditor::DrawSpawnRulesEditor()
{
    const glm::vec2 currentWindowSize = ImGui::GetContentRegionAvail();
    const glm::vec2 utilityPlace = currentWindowSize - 50.0f;
    ImGui::SetCursorPosX(((currentWindowSize - utilityPlace) * 0.5f).x);
    ImGui::BeginChild(ImGui::GetID("Peons templates list"), utilityPlace, true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    GameplayRulesManager::Instance().FPeonSpawningRulesManager.DrawEditor();

    ImGui::EndChild();
}

void GameRulesEditor::DrawBuildingCostsEditor()
{
    const glm::vec2 currentWindowSize = ImGui::GetContentRegionAvail();
    const glm::vec2 utilityPlace = currentWindowSize - 50.0f;
    ImGui::SetCursorPosX(((currentWindowSize - utilityPlace) * 0.5f).x);
    ImGui::BeginChild(ImGui::GetID("Building cost editor"), utilityPlace, true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    GameplayRulesManager::Instance().FBuildingCostManager.DrawEditor();

    ImGui::EndChild();
}

void GameRulesEditor::DrawConstantsEditor()
{
    const glm::vec2 currentWindowSize = ImGui::GetContentRegionAvail();
    const glm::vec2 utilityPlace = currentWindowSize - 50.0f;
    ImGui::SetCursorPosX(((currentWindowSize - utilityPlace) * 0.5f).x);
    ImGui::BeginChild(ImGui::GetID("Game rules editor"), utilityPlace, true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    GameplayRulesManager::Instance().DrawConstantsEditor();

    ImGui::EndChild();
}

void DrawGameRulesEditor(bool* parOpen, float parMenuBarHeight)
{
    static GameRulesEditor editor;
    editor.DrawEditor(parOpen, parMenuBarHeight);
}

} // namespace ImGUITools
} // namespace ECSEngine