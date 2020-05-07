#include "stdafx.h"

#include "ScenarioEditor.h"

#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"
#include "GameScenarioUpdater.h"
#include "ImGuiTools/SceneEditor.h"
#include "Rendering/EditorSceneRenderer.h"
#include "Rendering/SceneObjectsPickingRenderer.h"

namespace ECSEngine
{

ScenarioEditor::ScenarioEditor()
    : FCurrentScenario(nullptr)
    , FEditorSceneObjectPickingRenderer(nullptr)
    , FEditorSceneRenderer(nullptr)
{
}

ScenarioEditor::~ScenarioEditor()
{
    delete FCurrentScenario;
}

void ScenarioEditor::Initialise()
{
    FEditorSceneObjectPickingRenderer = new SceneObjectsPickingRenderer();
    FEditorSceneObjectPickingRenderer->Initialise();
    FEditorSceneRenderer = new EditorSceneRenderer();
    FEditorSceneRenderer->Initialise("meshes\\testobjects\\movehandle.fbx.gen", "materials\\vertexcolormaterial.material");

    FEditorCamera.Initialise();
}

void ScenarioEditor::Destroy()
{
    if (FCurrentScenario != nullptr)
        FCurrentScenario->Destroy();

    FEditorSceneRenderer->Shutdown();
    FEditorSceneObjectPickingRenderer->Shutdown();

    delete FEditorSceneRenderer;
    delete FEditorSceneObjectPickingRenderer;
}

void ScenarioEditor::Update()
{
    FEditorCamera.Update();

    if (!FIOScene.openScene)
        ImGUITools::DrawSceneEditorMainMenu(FCurrentScenario, FWindows, FIOScene);

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
        std::ofstream ofstr(GlobalResourceCache::Instance().FCache->GetBasePath() + "/Scenes/" + FCurrentScenario->GetName() + ".scene");
        cereal::JSONOutputArchive outputArchive(ofstr);

        outputArchive(*FCurrentScenario);
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
}

void ScenarioEditor::Render()
{
    const SceneScenario* currentScene = GetEditedScenario();

    if (currentScene != nullptr)
    {
        FEditorSceneObjectPickingRenderer->RenderScene(currentScene);
        FEditorSceneRenderer->RenderScene(currentScene);
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

} // namespace ECSEngine