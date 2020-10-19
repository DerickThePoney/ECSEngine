#include "stdafx.h"

#include "ApplicationSceneActions.h"

#include "Common/Camera.h"
#include "Common/CameraManager.h"
#include "Common/PolygonTriangulator.h"
#include "Common/RenderingHandles.h"
#include "PropertyDrawer.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/MaterialManager.h"
#include "SceneScenario.h"

CEREAL_REGISTER_TYPE(ECSEngine::SceneActionWithBaseSceneItem);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ISceneAction, ECSEngine::SceneActionWithBaseSceneItem)

CEREAL_REGISTER_TYPE(ECSEngine::SceneActionCreateMainCamera);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::SceneActionWithBaseSceneItem, ECSEngine::SceneActionCreateMainCamera)

CEREAL_REGISTER_TYPE(ECSEngine::SceneActionPolygonalPattern);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::SceneActionWithBaseSceneItem, ECSEngine::SceneActionPolygonalPattern)

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
    const glm::mat4 localToWorld = glm::translate(item->GetPosition()) * glm::eulerAngleXYZ(eulerAngles.x, eulerAngles.y, eulerAngles.z);
    const glm::mat4 worldViewMatrix = glm::inverse(localToWorld);

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

    const float aspectRatio = Rendering::GLFWDisplayWindowHandler::Instance().AspectRatio();
    const glm::mat4 perspectiveMatrix = glm::perspective(glm::radians(FFov), aspectRatio, FNearPlane, FFarPlane);
    const BaseSceneItem* item = GetSceneItem();
    if (item != nullptr)
    {
        const glm::vec3 eulerAngles = item->GetEulerAngles();
        const glm::mat4 localToWorld = glm::translate(item->GetPosition()) * glm::eulerAngleXYZ(eulerAngles.x, eulerAngles.y, eulerAngles.z);

        parCommandBuffer.DrawFrustum(parMaterial, localToWorld, perspectiveMatrix, true, 0xFF00FF00, 0xFF0000FF);
        return true;
    }
    return false;
}

/*************************************************************/
/*            SceneActionPolygonalPattern                    */
/*************************************************************/
IMPLEMENT_SCENE_ACTION(SceneActionPolygonalPattern);

SceneActionPolygonalPattern::SceneActionPolygonalPattern(const std::string& parFName /*= "Dummy"*/)
    : SceneActionWithBaseSceneItem(parFName)
{
}

SceneActionPolygonalPattern::~SceneActionPolygonalPattern()
{
}

void SceneActionPolygonalPattern::VirtualInitialise(const SceneScenario* parScene)
{
    SceneActionWithBaseSceneItem::VirtualInitialise(parScene);

    FPolygon.push_back(glm::vec2(0.f));
    FPolygon.push_back(glm::vec2(1.f, 1.5f));
    FPolygon.push_back(glm::vec2(2.5f, 0.f));
    FPolygon.push_back(glm::vec2(2.f, 1.0f));
    FPolygon.push_back(glm::vec2(3.f, 0.5f));
    FPolygon.push_back(glm::vec2(2.5f, 4.0f));
    FPolygon.push_back(glm::vec2(0.5f, 2.0f));

    PolygonTriangulator triangulator;
    FTriangles = triangulator.Triangulate(FPolygon);
}

void SceneActionPolygonalPattern::VirtualStart()
{
    SceneActionWithBaseSceneItem::VirtualStart();
}

void SceneActionPolygonalPattern::VirtualDrawEditor()
{
    SceneActionWithBaseSceneItem::VirtualDrawEditor();
}

bool SceneActionPolygonalPattern::VirtualDrawInSceneEditor(Rendering::DrawCommandBuffer& parCommandBuffer, Rendering::MaterialInstanceHandle& parMaterial)
{
    SceneActionWithBaseSceneItem::VirtualDrawInSceneEditor(parCommandBuffer, parMaterial);

    const glm::vec3 offset = (GetSceneItem() == nullptr) ? glm::vec3(0.f) : GetSceneItem()->GetPosition();
    vertices.clear();
    forrange(i, 0, FPolygon.size())
    {
        const glm::vec2& currentVertex = FPolygon[i];
        vertices.push_back(glm::vec3(currentVertex.x, 0.f, currentVertex.y) + offset);
    }

    parCommandBuffer.DrawLines(parMaterial, vertices.data(), (u32)vertices.size(), 0xFF00FF00, true);

    verticesForTriangles.clear();
    forrange(i, 0, FTriangles.size())
    {
        const Triangle2D& currentTri = FTriangles[i];
        std::vector<glm::vec3> verts;
        verts.push_back(glm::vec3(currentTri.A.x, 0.f, currentTri.A.y) + offset);
        verts.push_back(glm::vec3(currentTri.B.x, 0.f, currentTri.B.y) + offset);
        verts.push_back(glm::vec3(currentTri.C.x, 0.f, currentTri.C.y) + offset);
        verticesForTriangles.push_back(verts);
        parCommandBuffer.DrawLines(parMaterial, verticesForTriangles[verticesForTriangles.size() - 1].data(), 3, 0xFFFF0000, true);
    }

    return true;
}

} // namespace ECSEngine