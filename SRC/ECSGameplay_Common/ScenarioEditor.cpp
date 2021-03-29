#include "stdafx.h"

#include "ScenarioEditor.h"

#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"
#include "GameScenarioUpdater.h"
#include "ImGuiTools/SceneEditor.h"
#include "Rendering/EditorGridRenderer.h"
#include "Rendering/EditorSceneRenderer.h"
#include "Rendering/SceneObjectsPickingRenderer.h"
#include "RenderingCore/GFXOperator.h"
#include "RenderingCore/ImguiRenderer.h"
#include "RenderingCore/RenderPass.h"

namespace ECSEngine
{

ScenarioEditor::ScenarioEditor()
    : FCurrentScenario(nullptr)
    , FEditorSceneObjectPickingRenderer(nullptr)
    , FEditorSceneRenderer(nullptr)
    , FEditorGridRenderer(nullptr)
    , FInGameScenarioPlayer(nullptr)
    , FState(ScenarioEditorStatus::EDITING_SCENARIO)
{
}

ScenarioEditor::~ScenarioEditor()
{
    delete FCurrentScenario;
    delete FInGameScenarioPlayer;
}

void ScenarioEditor::Initialise()
{
    FEditorSceneObjectPickingRenderer = new SceneObjectsPickingRenderer();
    FEditorSceneObjectPickingRenderer->Initialise();
    FEditorSceneRenderer = new EditorSceneRenderer();
    FEditorSceneRenderer->Initialise("meshes\\testobjects\\movehandle_v2.fbx.gen", "materials\\vertexcolormaterial.material");
    FEditorGridRenderer = new EditorGridRenderer();
    FEditorGridRenderer->Initialise();

    FEditorCamera.Initialise();
}

void ScenarioEditor::Destroy()
{
    if (FCurrentScenario != nullptr)
        FCurrentScenario->Destroy();

    FEditorGridRenderer->Shutdown();
    FEditorSceneRenderer->Shutdown();
    FEditorSceneObjectPickingRenderer->Shutdown();

    if (FInGameScenarioPlayer != nullptr)
    {
        AlwaysCheckedAssert(FState == ScenarioEditorStatus::PLAYING_SCENARIO);
        FInGameScenarioPlayer->Destroy();
        delete FInGameScenarioPlayer;
        FInGameScenarioPlayer = nullptr;
    }

    delete FEditorGridRenderer;
    delete FEditorSceneRenderer;
    delete FEditorSceneObjectPickingRenderer;
}

void ScenarioEditor::Update()
{
    UpdateSceneEditorStatus();

    switch (FState)
    {
    case ECSEngine::ScenarioEditorStatus::EDITING_SCENARIO:
        UpdateForSceneEditing();
        break;
    case ECSEngine::ScenarioEditorStatus::PLAYING_SCENARIO:
        UpdateForInEditorPlaying();
        break;
    default:
        AssertNotReached();
        break;
    }
}

void ScenarioEditor::Render()
{
    switch (FState)
    {
    case ECSEngine::ScenarioEditorStatus::EDITING_SCENARIO:
        RenderForSceneEditing();
        break;
    case ECSEngine::ScenarioEditorStatus::PLAYING_SCENARIO:
        RenderForEditorPlaying();
        break;
    default:
        AssertNotReached();
        break;
    }
}

void ScenarioEditor::UpdateSelectedItems(const std::pair<u32, u32>& parSelectedItem, const bool parSelected, const bool parUnselect)
{
    SceneScenario* currentScene = GetEditedScenario();
    if (currentScene != nullptr)
    {
        currentScene->SetItemHovered(parSelectedItem.first);

        if (parSelected)
            currentScene->SetItemSelected(parSelectedItem.first);
        else if (parUnselect)
            currentScene->SetItemSelected(-1);
    }
}

void ScenarioEditor::UpdateForSceneEditing()
{
    Rendering::ImGUI::SetImGuiContext(Rendering::RenderPassId::IMGUI_EDITOR_PASS);
    FEditorCamera.Update();

    if (!FIOScene.openScene)
    {
        ImGUITools::DrawSceneEditorMainMenu(FCurrentScenario, FWindows, FIOScene);
    }

    if (FIOScene.newScene)
    {
        bool isDone = false;
        bool isCancel = false;
        const std::string sceneToChoose = ImGUITools::NewScenario(isDone, isCancel);

        AlwaysCheckedAssert(!(isDone && isCancel));

        if (isDone)
        {
            AlwaysCheckedAssert(!sceneToChoose.empty());
            if (FCurrentScenario != nullptr)
            {
                FCurrentScenario->Destroy();
                FCurrentScenario = nullptr;
            }

            FCurrentScenario = new SceneScenario();
            FCurrentScenario->SetName(sceneToChoose);
            FCurrentScenario->Initialise();
            FIOScene.newScene = false;
        }
        else if (isCancel)
        {
            FIOScene.newScene = false;
        }
    }

    if (FCurrentScenario != nullptr && FIOScene.saveScene)
    {
        const std::string filename = GlobalResourceCache::Instance().FCache->GetBasePath() + "/Scenes/" + FCurrentScenario->GetName() + ".scene";
        bool isNewScene = false;
        {
            std::ifstream ifstr(filename);
            isNewScene = !ifstr.good();
        }

        {
            std::ofstream ofstr(GlobalResourceCache::Instance().FCache->GetBasePath() + "/Scenes/" + FCurrentScenario->GetName() + ".scene");
            cereal::JSONOutputArchive outputArchive(ofstr);

            outputArchive(*FCurrentScenario);
        }

        if (isNewScene)
            GlobalResourceCache::Instance().FCache->ReOpenFileSystem();

        FIOScene.saveScene = false;
    }

    if (FIOScene.openScene)
    {

        bool isDone = false;
        bool isCancel = false;
        const std::string sceneToChoose = ImGUITools::ChooseScenario(isDone, isCancel);

        AlwaysCheckedAssert(!(isDone && isCancel));

        if (isDone)
        {
            if (FCurrentScenario != nullptr)
            {
                FCurrentScenario->Destroy();
                FCurrentScenario = nullptr;
            }

            Resource res(sceneToChoose);
            std::shared_ptr<ResourceHandle> handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&res);
            ResourceBuffer buff = handle->GetResourceBuffer();
            std::istream sstr(&buff, std::istream::in);

            cereal::JSONInputArchive archive(sstr);
            FCurrentScenario = new SceneScenario();
            archive(*FCurrentScenario);
            FIOScene.openScene = false;

            FCurrentScenario->Initialise();
        }
        else if (isCancel)
        {
            FIOScene.openScene = false;
        }
    }

    std::pair<u32, u32> selectedItem = FEditorSceneObjectPickingRenderer->GetPickedItemAndHits(0.0f);

    UpdateSelectedItems(selectedItem, Input::GetMouseButtonState(0), Input::GetMouseButtonState(1));

    if (FWindows.showPickingDebug)
        FEditorSceneObjectPickingRenderer->DrawDebugData(&FWindows.showPickingDebug);

    if (FWindows.showEditorCameraParameters)
        FEditorCamera.EditorWindow(&FWindows.showEditorCameraParameters);
}

void ScenarioEditor::UpdateForInEditorPlaying()
{
    AssertRelease(FInGameScenarioPlayer != nullptr);

    // TODO Editor Playing scene menu

    FInGameScenarioPlayer->Update();
}

void ScenarioEditor::UpdateSceneEditorStatus()
{
    Rendering::ImGUI::SetImGuiContext(Rendering::RenderPassId::IMGUI_EDITOR_PASS);
    if (!FIOScene.openScene)
    {
        bool isPlaying = FState == ScenarioEditorStatus::PLAYING_SCENARIO;
        ImGUITools::DrawPlayScenarioWindow(isPlaying);
        if (FCurrentScenario != nullptr)
        {
            if (isPlaying && FState != ScenarioEditorStatus::PLAYING_SCENARIO)
            {
                FState = ScenarioEditorStatus::PLAYING_SCENARIO;
                AssertRelease(FInGameScenarioPlayer == nullptr);
                FInGameScenarioPlayer = new GameScenarioUpdater();
                FInGameScenarioPlayer->SetScenario(GlobalResourceCache::Instance().FCache->GetBasePath() + "/Scenes/" + FCurrentScenario->GetName() + ".scene");
                FInGameScenarioPlayer->Initialise();
            }
            else if (!isPlaying && FState != ScenarioEditorStatus::EDITING_SCENARIO)
            {
                FState = ScenarioEditorStatus::EDITING_SCENARIO;
                AssertRelease(FInGameScenarioPlayer != nullptr);
                FInGameScenarioPlayer->Destroy();
                delete FInGameScenarioPlayer;
                FInGameScenarioPlayer = nullptr;
            }
        }
    }

    MemoryView<const char*> operators = Rendering::GFXOperatorDescriptorFactory::GetOperatorsList();
}

void ScenarioEditor::RenderForSceneEditing()
{
    const SceneScenario* currentScene = GetEditedScenario();

    if (currentScene != nullptr)
    {
        FEditorSceneObjectPickingRenderer->RenderScene(currentScene);
        FEditorSceneRenderer->RenderScene(currentScene);
    }

    FEditorGridRenderer->RenderScene();
}

void ScenarioEditor::RenderForEditorPlaying()
{
    AssertRelease(FInGameScenarioPlayer != nullptr);
    FInGameScenarioPlayer->Render();
}

} // namespace ECSEngine
