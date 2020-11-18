#include "stdafx.h"

#include "SceneEditor.h"

#include "Application/PropertyDrawer.h"
#include "Application/SceneScenario.h"
#include "Common/Logger.h"
#include "Common/RenderingHandles.h"
#include "Common/ResourceCache.h"
#include "Common/Singleton.h"
#include "ECSGameplay_Common/GameplaySceneActions.h"
#include "ECSGameplay_Common/ScenarioEditor.h"
#include "EntityTemplatesEditor.h"
#include "InputDebug.h"
#include "LoggerGUI.h"
#include "RenderingCore/BGFXRenderer.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/ImguiRenderer.h"
#include "RenderingCore/TexturesManager.h"

namespace ECSEngine
{
namespace ImGUITools
{
namespace
{
void MainMenuBar(WindowsToShow& options, glm::vec2& parOutMenuBarHeight, IOScene& parOutIOScene)
{
    ImGui::BeginMainMenuBar();
    parOutMenuBarHeight = ImGui::GetWindowSize();
    if (ImGui::BeginMenu("File"))
    {
        ImGui::MenuItem("New scenario", NULL, &parOutIOScene.newScene);
        ImGui::MenuItem("Open scenario", NULL, &parOutIOScene.openScene);
        ImGui::MenuItem("Save scenario", NULL, &parOutIOScene.saveScene);
        ImGui::Separator();
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Show"))
    {
        ImGui::MenuItem("Scene items", NULL, &options.showSceneItemsList);
        ImGui::MenuItem("Scene actions", NULL, &options.showActionManager);
        ImGui::Separator();
        ImGui::MenuItem("Scene templates", NULL, &options.showEntityTemplateEditor);
        ImGui::Separator();
        if (ImGui::BeginMenu("Debug windows"))
        {
            ImGui::MenuItem("Imgui Demo", NULL, &options.showImGuiDemo);
            ImGui::MenuItem("Input debug", NULL, &options.showInputDebug);
            ImGui::Separator();
            ImGui::MenuItem("Picking debug", NULL, &options.showPickingDebug);
            ImGui::Separator();
            ImGui::MenuItem("BGFX statistics", NULL, &options.showBGFXStatistics);
            ImGui::EndMenu();
        }
        ImGui::Separator();
        ImGui::MenuItem("Scene logger", NULL, &options.showLogger);
        ImGui::Separator();
        ImGui::MenuItem("Editor camera", NULL, &options.showEditorCameraParameters);

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Tools"))
    {
        ImGui::MenuItem("UI Style Editor", NULL, &options.showUIStyleEditor);
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
}

void SceneItemsWindow(SceneScenario* parScene, WindowsToShow& options, const glm::vec2& menuBarHeight)
{
    const glm::vec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    const float menuBarProportion = menuBarHeight.y / (float)windowSize.y;
    const glm::vec2 sceneItemsSize = windowSize * glm::vec2(0.25f, 1.f - menuBarProportion);
    ImGui::SetNextWindowSize(sceneItemsSize);
    ImGui::SetNextWindowPos(glm::vec2(0.0f, menuBarHeight.y));
    ImGui::Begin("Scene items", &options.showSceneItemsList, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    if (parScene != nullptr)
    {
        std::string sceneName = parScene->GetName();
        EDITOR_PROPERTY_STRING("Scene Name", sceneName, false, "");
        parScene->SetName(sceneName);

        if (ImGui::Button("Add Scene item"))
        {
            parScene->AddSceneItem(SceneItemTraits<BaseSceneItem>::GetSceneItemTypeId());
        }

        const glm::vec2 contentRegion = ImGui::GetContentRegionAvail();
        const glm::vec2 listSize = contentRegion - glm::vec2(10.0f, 0.0f);
        const float xCursor = ((contentRegion - listSize) * 0.5f).x;

        ImGui::SetCursorPosX(xCursor);

        ImGui::BeginChildFrame(ImGui::GetID("Scene items list"), listSize);

        SceneItemsContainer& sceneItems = parScene->GetSceneItemsForWriting();
        u32 i = 0;

        SceneItemsContainer::iterator itToErase = sceneItems.end();
        for (auto sceneItem = sceneItems.begin(); sceneItem != sceneItems.end(); ++sceneItem)
        {
            ImGui::PushID(i);
            if (ImGui::Button("X"))
            {
                itToErase = sceneItem;
            }
            ImGui::SameLine();
            sceneItem->second->DrawEditor();
            ImGui::PopID();
            i++;
        }
        if (itToErase != sceneItems.end())
        {
            parScene->RemoveSceneItem(itToErase);
        }

        ImGui::EndChildFrame();
    }
    ImGui::End();
}

void SceneActionsWindow(SceneScenario* parScene, WindowsToShow& options, const glm::vec2& menuBarHeight)
{
    const glm::vec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    const float menuBarProportion = menuBarHeight.y / (float)windowSize.y;
    const glm::vec2 sceneActionsSize = windowSize * glm::vec2(0.25f, 1.f - menuBarProportion);
    ImGui::SetNextWindowSize(sceneActionsSize);
    ImGui::SetNextWindowPos(glm::vec2(windowSize.x - sceneActionsSize.x, menuBarHeight.y));
    ImGui::Begin("Scene actions", &options.showActionManager, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    if (parScene != nullptr)
    {
        std::vector<std::shared_ptr<ISceneAction>>& sceneActions = parScene->GetSceneActionsForWriting();
        u32 i = 0;

        std::vector<std::shared_ptr<ISceneAction>>::iterator itToErase = sceneActions.end();
        u32 action = -1; // 0 erase / 1 up / 2 down
        for (auto sceneAction = sceneActions.begin(); sceneAction != sceneActions.end(); ++sceneAction)
        {
            ImGui::PushID(i);
            if (ImGui::Button("X"))
            {
                itToErase = sceneAction;
                action = 0;
            }
            ImGui::SameLine();
            if (i > 0 && ImGui::Button("UP"))
            {
                itToErase = sceneAction;
                action = 1;
            }
            ImGui::SameLine();
            if (i < ((u32)sceneActions.size() - 1) && ImGui::Button("DOWN"))
            {
                itToErase = sceneAction;
                action = 2;
            }
            ImGui::SameLine();
            (*sceneAction)->DrawEditor();
            ImGui::PopID();
            i++;
        }
        if (itToErase != sceneActions.end())
        {
            switch (action)
            {
            case 0:
            {
                sceneActions.erase(itToErase);
                break;
            }
            case 1:
            {
                auto previousIt = itToErase - 1;
                std::iter_swap(itToErase, previousIt);
                break;
            }
            case 2:
            {
                auto nextIt = itToErase + 1;
                std::iter_swap(itToErase, nextIt);
                break;
            }
            default:
                AssertNotReached();
            }
        }

        ImGui::Separator();

        const std::map<u32, std::string> actionList = SceneActionManagement::GetSceneActionsList();
        static u32 selectedAction = -1;
        if (ImGui::BeginCombo("##ActionsList", (selectedAction != -1) ? actionList.at(selectedAction).c_str() : ""))
        {
            foreachitemconst(action, actionList)
            {
                bool is_selected = (selectedAction == action.first);
                if (ImGui::Selectable(action.second.c_str(), is_selected))
                    selectedAction = action.first;
                if (is_selected)
                    ImGui::SetItemDefaultFocus(); // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        if (ImGui::Button("Add action") && selectedAction != -1)
        {
            auto action = actionList.find(selectedAction);
            AssertRelease(action != actionList.end());
            ISceneAction* newAction = SceneActionManagement::CreateSceneAction(selectedAction);
            AssertRelease(newAction != nullptr);
            newAction->SetName("New action");
            parScene->AddSceneActionStealOwnership(newAction);
        }
    }
    ImGui::End();
}

void UIStyleEditor(bool* parOpen)
{
    static Rendering::RenderPassId::Type passToEdit = Rendering::RenderPassId::IMGUI_PASSES_START;

    Rendering::ImGUI::SetImGuiContext(passToEdit);

    ImGui::Begin("UI Style Editor", parOpen);
    if (ImGui::BeginCombo("ImGui context style to edit", Rendering::RenderPassId::GetName(passToEdit)))
    {
        for (u16 i = Rendering::RenderPassId::IMGUI_PASSES_START; i <= Rendering::RenderPassId::IMGUI_PASSES_END; ++i)
        {
            bool selected = i == passToEdit;
            if (ImGui::Selectable(Rendering::RenderPassId::GetName((Rendering::RenderPassId::Type)i), selected))
                passToEdit = (Rendering::RenderPassId::Type)i;

            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    // TODO GET CURRENT STYLE, UPDATE THE STYLE OF THE WINDOWS, SAVE IT AND THEN SETUP THE STYLES CORRECTLY
    ImGui::ShowStyleEditor();

    ImGui::End();
    Rendering::ImGUI::SetImGuiContext(Rendering::RenderPassId::IMGUI_EDITOR_PASS);
}

} // namespace

void DrawSceneEditorMainMenu(SceneScenario* parScene, WindowsToShow& parOutWindowsToShow, IOScene& parOutIOScene)
{

    glm::vec2 menuBarHeight;
    MainMenuBar(parOutWindowsToShow, menuBarHeight, parOutIOScene);

    if (parOutWindowsToShow.showSceneItemsList)
        SceneItemsWindow(parScene, parOutWindowsToShow, menuBarHeight);

    if (parOutWindowsToShow.showActionManager)
        SceneActionsWindow(parScene, parOutWindowsToShow, menuBarHeight);

    if (parOutWindowsToShow.showEntityTemplateEditor)
        DrawEntityTemplatesEditor(&parOutWindowsToShow.showEntityTemplateEditor, menuBarHeight.y);

    if (parOutWindowsToShow.showLogger)
        DrawLogger(Logger::GetLoggedMessages(), true, &parOutWindowsToShow.showLogger);

    if (parOutWindowsToShow.showImGuiDemo)
        ImGui::ShowDemoWindow(&parOutWindowsToShow.showImGuiDemo);

    if (parOutWindowsToShow.showInputDebug)
        InputDebug(&parOutWindowsToShow.showInputDebug);

    if (parOutWindowsToShow.showBGFXStatistics)
        Rendering::BGFXRenderer::Instance().DrawStats(&parOutWindowsToShow.showBGFXStatistics);

    if (parOutWindowsToShow.showUIStyleEditor)
        UIStyleEditor(&parOutWindowsToShow.showUIStyleEditor);
}

void DrawPlayScenarioWindow(bool& parOutPlayScenario)
{
    static Rendering::TextureName playButton("UI", "PlayButton");
    static Rendering::TextureName pauseButton("UI", "StopButton");

    static Rendering::TextureHandle playHandle = Rendering::TextureManager::Instance().GetTextureHandle(playButton);
    static Rendering::TextureHandle pauseHandle = Rendering::TextureManager::Instance().GetTextureHandle(pauseButton);

    AssertRelease(playHandle.IsValid());
    AssertRelease(pauseHandle.IsValid());

    const glm::vec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();

    glm::vec2 menuBarHeight(0.f);
    if (!parOutPlayScenario)
    {
        ImGui::BeginMainMenuBar();
        menuBarHeight = ImGui::GetWindowSize();
        ImGui::EndMainMenuBar();
    }

    ImGui::Begin("PlayButton", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
    const float thisWindowSize = ImGui::GetWindowWidth();
    ImGui::SetWindowPos(glm::vec2((windowSize.x - thisWindowSize) * 0.5f, menuBarHeight.y));
    if (parOutPlayScenario)
    {
        if (ImGui::ImageButton((ImTextureID)&pauseHandle, glm::vec2(64, 64)))
            parOutPlayScenario = false;
    }
    else
    {
        if (ImGui::ImageButton((ImTextureID)&playHandle, glm::vec2(64, 64)))
            parOutPlayScenario = true;
    }
    ImGui::End();
}

const std::string NewScenario(bool& isOk, bool& isCancel)
{
    ImGui::Begin("Choose Scenario Name...", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    static std::string sceneToChoose;
    EDITOR_PROPERTY_STRING("New scenario name", sceneToChoose, false, "*.scene");
    ImGui::SameLine();
    if (ImGui::Button("Ok"))
        isOk = true;
    ImGui::SameLine();
    if (ImGui::Button("Cancel"))
        isCancel = true;
    ImGui::End();

    return sceneToChoose;
}

const std::string ChooseScenario(bool& isOk, bool& isCancel)
{
    ImGui::Begin("Choose Scenario...", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    static std::string sceneToChoose;
    EDITOR_PROPERTY_STRING("Scenario to open", sceneToChoose, true, "*.scene");
    ImGui::SameLine();
    if (ImGui::Button("Ok"))
        isOk = true;
    ImGui::SameLine();
    if (ImGui::Button("Cancel"))
        isCancel = true;
    ImGui::End();

    return sceneToChoose;
}

} // namespace ImGUITools
} // namespace ECSEngine