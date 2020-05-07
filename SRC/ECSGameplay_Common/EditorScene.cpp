#include "stdafx.h"

#include "EditorScene.h"

#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"
#include "ImGuiTools/SceneEditor.h"

namespace ECSEngine
{

EditorScene::EditorScene()
    : Scene()
    , FCurrentScene(nullptr)
{
    SetName("Editor Scene");
}

EditorScene::~EditorScene()
{
    delete FCurrentScene;
}

void EditorScene::Initialise()
{
    FEditorCamera.Initialise();
}

void EditorScene::Destroy()
{
    if (FCurrentScene != nullptr)
        FCurrentScene->Destroy();
}

void EditorScene::Update()
{
    FEditorCamera.Update();

    if (!FIOScene.openScene)
        ImGUITools::DrawSceneEditorMainMenu(FCurrentScene, FWindows, FIOScene);

    if (FIOScene.newScene)
    {
        bool isDone = false;
        bool isCancel = false;
        const std::string sceneToChoose = ImGUITools::NewScene(isDone, isCancel);

        AlwaysCheckedAssert(!(isDone && isCancel));

        if (isDone)
        {
            AlwaysCheckedAssert(!sceneToChoose.empty());
            if (FCurrentScene != nullptr)
            {
                FCurrentScene->Destroy();
                FCurrentScene = nullptr;
            }

            FCurrentScene = new Scene();
            FCurrentScene->SetName(sceneToChoose);
            FCurrentScene->Initialise();
            FIOScene.newScene = false;
        }
        else if (isCancel)
        {
            FIOScene.newScene = false;
        }
    }

    if (FCurrentScene != nullptr && FIOScene.saveScene)
    {
        std::ofstream ofstr(GlobalResourceCache::Instance().FCache->GetBasePath() + "/Scenes/" + FCurrentScene->GetName() + ".scene");
        cereal::JSONOutputArchive outputArchive(ofstr);

        outputArchive(*FCurrentScene);
        FIOScene.saveScene = false;
    }

    if (FIOScene.openScene)
    {

        bool isDone = false;
        bool isCancel = false;
        const std::string sceneToChoose = ImGUITools::ChooseScene(isDone, isCancel);

        AlwaysCheckedAssert(!(isDone && isCancel));

        if (isDone)
        {
            if (FCurrentScene != nullptr)
            {
                FCurrentScene->Destroy();
                FCurrentScene = nullptr;
            }

            Resource res(sceneToChoose);
            std::shared_ptr<ResourceHandle> handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&res);
            ResourceBuffer buff = handle->GetResourceBuffer();
            std::istream sstr(&buff, std::istream::in);

            cereal::JSONInputArchive archive(sstr);
            FCurrentScene = new Scene();
            archive(*FCurrentScene);
            FIOScene.openScene = false;

            FCurrentScene->Initialise();
        }
        else if (isCancel)
        {
            FIOScene.openScene = false;
        }
    }
}

void EditorScene::Render()
{
}

void EditorScene::UpdateSelectedItems(const std::pair<u32, u32>& parSelectedItem, const bool parSelected, const bool parUnselect)
{
    Scene* currentScene = GetEditedScene();
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