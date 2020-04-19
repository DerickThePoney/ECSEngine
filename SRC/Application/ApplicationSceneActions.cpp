#include "stdafx.h"

#include "ApplicationSceneActions.h"

#include "PropertyDrawer.h"
#include "Scene.h"

CEREAL_REGISTER_TYPE(ECSEngine::SceneActionWithBaseSceneItem);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ISceneAction, ECSEngine::SceneActionWithBaseSceneItem)

namespace ECSEngine
{

/*************************************************************/
/*            SceneActionWithBaseSceneItem                   */
/*************************************************************/
SceneActionWithBaseSceneItem::SceneActionWithBaseSceneItem(const std::string& parFName /*= "Dummy"*/)
    : ISceneAction(parFName)
{
}

SceneActionWithBaseSceneItem::~SceneActionWithBaseSceneItem()
{
}

void SceneActionWithBaseSceneItem::SetSceneItem(const BaseSceneItem* parSceneItem)
{
    FSceneItem = parSceneItem;
    FSceneItemID = parSceneItem->GetSceneItemTypeId();
}

void SceneActionWithBaseSceneItem::VirtualInitialise(const Scene* parScene)
{
    ISceneAction::VirtualInitialise(parScene);
    auto sceneItems = GetScene()->GetSceneItems();
    if (FSceneItemID < sceneItems.size())
    {
        FSceneItem = sceneItems[FSceneItemID].get();
        AssertRelease(FSceneItem != nullptr);
    }
}

void SceneActionWithBaseSceneItem::VirtualStart()
{
    ISceneAction::VirtualStart();
}

void SceneActionWithBaseSceneItem::VirtualDrawEditor()
{
    ISceneAction::VirtualDrawEditor();

    if (ShouldShowEditor())
    {
        EDITOR_PROPERTY_SCENE_ITEM("Scene item", FSceneItem, FSceneItemID, GetScene());
    }
}

} // namespace ECSEngine