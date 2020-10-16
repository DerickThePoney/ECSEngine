#include "stdafx.h"

#include "SceneScenario.h"

#include "ECSGameplay_Common/GameplaySceneActions.h"
#include "SceneItems.h"

namespace ECSEngine
{

SceneScenario::SceneScenario()
{
}

SceneScenario::~SceneScenario()
{
}

void SceneScenario::AddSceneItem(const u32 parSceneItemTypeId)
{
    const u32 nextId = FSceneItemsIdGenerator.GetNextId();
    std::shared_ptr<BaseSceneItem> sceneItem = std::shared_ptr<BaseSceneItem>(new BaseSceneItem("New scene item", nextId));
    FSceneItems.push_back(sceneItem);
    sceneItem->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    sceneItem->SetEulerAngles(glm::vec3(0.0f));
}

void SceneScenario::RemoveSceneItem(const std::vector<std::shared_ptr<BaseSceneItem>>::iterator parWhere)
{
    AssertRelease(parWhere != FSceneItems.end());
    const u32 id = (*parWhere)->Id();
    FSceneItemsIdGenerator.ReleaseId(id);
    FSceneItems.erase(parWhere);
}

void SceneScenario::Initialise()
{
    FCurrentAction = 0;
    // FActions.push_back(std::unique_ptr<SpawnEntitySceneAction>(new SpawnEntitySceneAction));
    foreachitem(action, FActions) action->Initialise(this);
}

void SceneScenario::Destroy()
{
    FActions.clear();
    FSceneItems.clear();
}

void SceneScenario::Update()
{
    bool shouldContinue = true;
    while (FCurrentAction < FActions.size() && shouldContinue)
    {
        ISceneAction* action = FActions[FCurrentAction].get();
        AssertRelease(action != nullptr);

        if (!action->IsStarted())
        {
            action->Start();
            AlwaysCheckedAssert(action->IsStarted());
        }

        if (!action->IsFinished())
            action->Update();

        if (action->IsFinished())
        {
            FCurrentAction++;
        }
        else
        {
            shouldContinue = false;
        }
    }
}

void SceneScenario::SetItemHovered(const u32 parId)
{
    foreachitem(sceneItem, FSceneItems) { sceneItem->SetItemHovered(sceneItem->Id() == parId); }
}

void SceneScenario::SetItemSelected(const u32 parId)
{
    foreachitem(sceneItem, FSceneItems) { sceneItem->SetItemSelected(sceneItem->Id() == parId); }
}

void SceneScenario::AddSceneActionStealOwnership(ISceneAction* parAction)
{
    AssertRelease(parAction != nullptr);
    parAction->Initialise(this);
    FActions.push_back(std::unique_ptr<ISceneAction>(parAction));
}

} // namespace ECSEngine