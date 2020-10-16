#include "stdafx.h"

#include "ApplicationSceneActions.h"

#include "Common/Camera.h"
#include "Common/CameraManager.h"
#include "Common/RenderingHandles.h"
#include "PropertyDrawer.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "SceneScenario.h"

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

void SceneActionWithBaseSceneItem::VirtualInitialise(const SceneScenario* parScene)
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

void SceneActionCreateMainCamera::VirtualInitialise(const SceneScenario* parScene)
{
    SceneActionWithBaseSceneItem::VirtualInitialise(parScene);

    const BaseSceneItem* item = GetSceneItem();
    if (item == nullptr)
        return;

    const glm::vec3 eulerAngles = item->GetEulerAngles();
    const glm::mat4 worldViewMatrix = glm::eulerAngleXYZ(eulerAngles.x, eulerAngles.y, eulerAngles.z) * glm::translate(-item->GetPosition());

    const u32 camId = CameraManager::Instance().CreateCameraIFN(FCameraName);
    Camera* cam = CameraManager::Instance().GetCamera(camId);
    cam->Init(worldViewMatrix, glm::radians(FFov), FNearPlane, FFarPlane);
}

void SceneActionCreateMainCamera::VirtualStart()
{
    SceneActionWithBaseSceneItem::VirtualStart();

    Finish();
}

void SceneActionCreateMainCamera::VirtualDrawEditor()
{
    SceneActionWithBaseSceneItem::VirtualDrawEditor();

    if (ShouldShowEditor())
    {
        EDITOR_PROPERTY_STRING("Camera name", FCameraName, false, "");
        EDITOR_PROPERTY_WITH_LIMITS("FoV", FFov, 0.0f, 180.0f);
        EDITOR_PROPERTY_WITH_LIMITS("Near Plane", FNearPlane, 0.0f, FFarPlane);
        EDITOR_PROPERTY_WITH_LIMITS("Far Plane", FFarPlane, FNearPlane, 10000.0f);
    }
}

bool SceneActionCreateMainCamera::VirtualDrawInSceneEditor(Rendering::DrawCommandBuffer& parCommandBuffer, Rendering::MaterialInstanceHandle& parMaterial)
{
    SceneActionWithBaseSceneItem::VirtualDrawInSceneEditor(parCommandBuffer, parMaterial);

    const glm::uvec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    const glm::mat4 perspectiveMatrix = glm::perspective(glm::radians(FFov), float(windowSize.x) / float(windowSize.y), FNearPlane, FFarPlane);
    const BaseSceneItem* item = GetSceneItem();
    if (item != nullptr)
    {
        const glm::vec3 eulerAngles = item->GetEulerAngles();
        const glm::mat4 worldViewMatrix = glm::eulerAngleXYZ(eulerAngles.x, eulerAngles.y, eulerAngles.z) * glm::translate(-item->GetPosition());

        parCommandBuffer.DrawFrustum(parMaterial, worldViewMatrix, perspectiveMatrix, 0xFF00FF00);
        return true;
    }
    return false;
}

} // namespace ECSEngine