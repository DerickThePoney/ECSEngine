#include "stdafx.h"

#include "Scene.h"

#include "ECSCore/ECSCoreSceneActions.h"
#include "ImGuiTools/SceneEditor.h" // TOREMOVE
#include "SceneItems.h"

namespace ECSEngine
{

Scene::Scene()
{
}

Scene::~Scene()
{
    forrange(i, 0, FActions.size()) delete FActions[i];
}

void Scene::AddSceneItem(const u32 parSceneItemTypeId)
{
}

void Scene::RemoveSceneItem(const u32 parId)
{
}

void Scene::Initialise()
{
    std::shared_ptr<BaseSceneItem> sceneItem = std::shared_ptr<BaseSceneItem>(new BaseSceneItem("Test Start Pos"));
    const u32 typeId = sceneItem->GetSceneItemTypeId();
    FSceneItems[typeId].push_back(sceneItem);
    sceneItem->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    sceneItem->SetOrientation(glm::quatLookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

    FCurrentAction = 0;

    SpawnEntitySceneAction* action = new SpawnEntitySceneAction("Entity spawning");
    action->SetSceneItem(sceneItem.get());
    action->SetEntityTemplateName("Template_Joueur");
    action->Initialise();
    FActions.push_back(action);
}

void Scene::Destroy()
{
    forrange(i, 0, FActions.size()) delete FActions[i];
    FActions.clear();
    FSceneItems.clear();
}

void Scene::OnDrawEditor()
{
    ImGUITools::DrawSceneEditor();
}

void Scene::Update()
{
    if (FCurrentAction >= FActions.size())
        return;

    ISceneAction* action = FActions[FCurrentAction];
    AssertRelease(action != nullptr);

    if (!action->IsStarted())
    {
        action->Start();
        AlwaysCheckedAssert(action->IsStarted());
    }

    action->Update();

    if (action->IsFinished())
    {
        FCurrentAction++;
    }
}

void Scene::Render()
{
}

} // namespace ECSEngine