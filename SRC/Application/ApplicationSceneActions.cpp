#include "stdafx.h"

#include "ApplicationSceneActions.h"

#include "PropertyDrawer.h"
#include "Scene.h"

CEREAL_REGISTER_TYPE(ECSEngine::SceneActionWithBaseSceneItem);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ISceneAction, ECSEngine::SceneActionWithBaseSceneItem)

CEREAL_REGISTER_TYPE(ECSEngine::SceneActionCreateMainCamera);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::SceneActionWithBaseSceneItem, ECSEngine::SceneActionCreateMainCamera)

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

void SceneActionWithBaseSceneItem::VirtualDrawEditor()
{
    ISceneAction::VirtualDrawEditor();

    if (ShouldShowEditor())
    {
        EDITOR_PROPERTY_SCENE_ITEM("Scene item", FSceneItem, FSceneItemID, GetScene());
    }
}

/*************************************************************/
/*            SceneActionCreateMainCamera                    */
/*************************************************************/

IMPLEMENT_SCENE_ACTION(SceneActionCreateMainCamera);

SceneActionCreateMainCamera::SceneActionCreateMainCamera(const std::string& parFName)
    : SceneActionWithBaseSceneItem(parFName)
{
}

SceneActionCreateMainCamera::~SceneActionCreateMainCamera()
{
}

void SceneActionCreateMainCamera::VirtualInitialise(const Scene* parScene)
{
    SceneActionWithBaseSceneItem::VirtualInitialise(parScene);
}

void SceneActionCreateMainCamera::VirtualStart()
{
    SceneActionWithBaseSceneItem::VirtualStart();
}

void SceneActionCreateMainCamera::VirtualDrawEditor()
{
    SceneActionWithBaseSceneItem::VirtualDrawEditor();

    if (ShouldShowEditor())
    {
        EDITOR_PROPERTY_WITH_LIMITS("FoV", FFov, 0.0f, 180.0f);
        EDITOR_PROPERTY_WITH_LIMITS("Near Plane", FNearPlane, 0.0f, FFarPlane);
        EDITOR_PROPERTY_WITH_LIMITS("Far Plane", FFarPlane, FNearPlane, 10000.0f);
    }
}

} // namespace ECSEngine