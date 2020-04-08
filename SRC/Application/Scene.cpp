#include "stdafx.h"

#include "Scene.h"

#include "ECSCore/ECSCoreSceneActions.h"
#include "SceneItems.h"

namespace ECSEngine
{

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::AddSceneItem(const u32 parSceneItemTypeId)
{
    const u32 nextId = FSceneItemsIdGenerator.GetNextId();
    std::shared_ptr<BaseSceneItem> sceneItem = std::shared_ptr<BaseSceneItem>(new BaseSceneItem("New scene item", nextId));
    FSceneItems.push_back(sceneItem);
    sceneItem->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    sceneItem->SetOrientation(glm::quatLookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
}

void Scene::Initialise()
{
    FCurrentAction = 0;
    // FActions.push_back(std::unique_ptr<SpawnEntitySceneAction>(new SpawnEntitySceneAction));
    foreachitem(action, FActions) action->Initialise(this);
}

void Scene::Destroy()
{
    FActions.clear();
    FSceneItems.clear();
}

void Scene::Update()
{
    if (FCurrentAction >= FActions.size())
        return;

    ISceneAction* action = FActions[FCurrentAction].get();
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

void Scene::AddSceneActionStealOwnership(ISceneAction* parAction)
{
    AssertRelease(parAction != nullptr);
    parAction->Initialise(this);
    FActions.push_back(std::unique_ptr<ISceneAction>(parAction));
}

} // namespace ECSEngine