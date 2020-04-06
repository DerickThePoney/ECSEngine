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
    /*FCurrentScene = new Scene();
    FCurrentScene->Initialise();*/
}

void EditorScene::Destroy()
{
    if (FCurrentScene != nullptr)
        FCurrentScene->Destroy();
}

void EditorScene::Update()
{
    if (!FIOScene.openScene)
        ImGUITools::DrawSceneEditorMainMenu(FCurrentScene, FWindows, FIOScene);

    if (FCurrentScene != nullptr && FIOScene.saveScene)
    {
        std::ofstream ofstr(GlobalResourceCache::Instance().FCache->GetBasePath() + "/Scenes/" + FCurrentScene->GetName() + ".scene");
        cereal::JSONOutputArchive outputArchive(ofstr);

        outputArchive(*FCurrentScene);
        FIOScene.saveScene = false;
    }

    if (FIOScene.openScene)
    {
        if (FCurrentScene != nullptr)
        {
            FCurrentScene->Destroy();
            FCurrentScene = nullptr;
        }

        bool isDone = false;
        bool isCancel = false;
        const std::string sceneToChoose = ImGUITools::ChooseScene(isDone, isCancel);

        AlwaysCheckedAssert(!(isDone && isCancel));

        if (isDone)
        {
            Resource res(sceneToChoose);
            std::shared_ptr<ResourceHandle> handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&res);
            ResourceBuffer buff = handle->GetResourceBuffer();
            std::istream sstr(&buff, std::istream::in);

            cereal::JSONInputArchive archive(sstr);
            FCurrentScene = new Scene();
            archive(*FCurrentScene);
            FIOScene.openScene = false;
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

} // namespace ECSEngine