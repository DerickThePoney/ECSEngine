#include "stdafx.h"

#include "ApplicationSceneActions.h"

#include "Common/Camera.h"
#include "Common/CameraManager.h"
#include "Common/IntersectionRoutines.h"
#include "Common/PolygonPartitionner.h"
#include "Common/PolygonTriangulator.h"
#include "Common/Ray.h"
#include "Common/RenderingHandles.h"
#include "Common/Segment.h"
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
    FSceneItemID = parSceneItem->Id();
}

void SceneActionWithBaseSceneItem::VirtualInitialise(const SceneScenario* parScene)
{
    ISceneAction::VirtualInitialise(parScene);
    auto sceneItems = GetScene()->GetSceneItems();
    auto itFind = sceneItems.find(FSceneItemID);
    if (itFind != sceneItems.end())
    {
        FSceneItem = itFind->second.get();
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
    : parent_type(parFName)
{
}

SceneActionCreateMainCamera::~SceneActionCreateMainCamera()
{
}

void SceneActionCreateMainCamera::VirtualInitialise(const SceneScenario* parScene)
{
    parent_type::VirtualInitialise(parScene);

    const BaseSceneItem* item = GetSceneItem();
    if (item == nullptr)
        return;

    const glm::vec3 eulerAngles = item->GetEulerAngles();
    const glm::mat4 localToWorld = glm::translate(item->GetPosition()) * glm::eulerAngleXYZ(eulerAngles.x, eulerAngles.y, eulerAngles.z);
    const glm::mat4 worldViewMatrix = glm::inverse(localToWorld);

    const u32 camId = CameraManager::Instance().CreateCameraIFN(FCameraName);
    Camera* cam = CameraManager::Instance().GetCamera(camId);
    cam->Init(worldViewMatrix, FFov, FNearPlane, FFarPlane);
}

void SceneActionCreateMainCamera::VirtualStart()
{
    parent_type::VirtualStart();

    Finish();
}

void SceneActionCreateMainCamera::VirtualDrawEditor()
{
    parent_type::VirtualDrawEditor();

    if (ShouldShowEditor())
    {
        EDITOR_PROPERTY_STRING("Camera name", FCameraName, false, "");
        EDITOR_PROPERTY_ANGLE("FoV", FFov, 0.0f, 180.0f);
        EDITOR_PROPERTY_WITH_LIMITS("Near Plane", FNearPlane, 0.0f, FFarPlane);
        EDITOR_PROPERTY_WITH_LIMITS("Far Plane", FFarPlane, FNearPlane, 10000.0f);
    }
}

bool SceneActionCreateMainCamera::VirtualDrawInSceneEditor(Rendering::DrawCommandBuffer& parCommandBuffer, Rendering::MaterialInstanceHandle& parMaterial)
{
    parent_type::VirtualDrawInSceneEditor(parCommandBuffer, parMaterial);

    const float aspectRatio = Rendering::GLFWDisplayWindowHandler::Instance().AspectRatio();
    const glm::mat4 perspectiveMatrix = glm::perspective(FFov, aspectRatio, FNearPlane, FFarPlane);
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
    : parent_type(parFName)
{
    FPolygon.append(std::vector<glm::vec2>({ glm::vec2(0.f, 0.f), glm::vec2(1.f, 0.f), glm::vec2(0.f, 1.f) }));
}

SceneActionPolygonalPattern::~SceneActionPolygonalPattern()
{
}

void SceneActionPolygonalPattern::VirtualInitialise(const SceneScenario* parScene)
{
    parent_type::VirtualInitialise(parScene);

    ComputeTriangulation();
}

void SceneActionPolygonalPattern::VirtualStart()
{
    parent_type::VirtualStart();
}

void SceneActionPolygonalPattern::VirtualDrawEditor()
{
    parent_type::VirtualDrawEditor();

    if (ShouldShowEditor())
    {
        if (ImGui::Button("Triangulate"))
        {
            ComputeTriangulation();
        }

        EDITOR_PROPERTY_VECTOR(glm::vec2, "Polygon points", FPolygon.data(), false);

        if (ImGui::CollapsingHeader("Polygon holes"))
        {
            ImGui::Indent();
            if (ImGui::Button("Add hole"))
            {
                Polygon2D p;
                p.push_back(glm::vec2(0.f));
                p.push_back(glm::vec2(0.f));
                p.push_back(glm::vec2(0.f));
                FPolygonHoles.push_back(p);
            }
            std::vector<Polygon2D>::iterator itToErase = FPolygonHoles.end();
            u32 i = 0;
            u32 action = -1; // 0 erase / 1 up / 2 down
            for (auto polygon = FPolygonHoles.begin(); polygon != FPolygonHoles.end(); ++polygon)
            {
                ImGui::PushID((u32)i);
                if (ImGui::Button("X"))
                {
                    itToErase = polygon;
                    action = 0;
                }
                ImGui::SameLine();
                if (i > 0 && ImGui::Button("UP"))
                {
                    itToErase = polygon;
                    action = 1;
                }
                ImGui::SameLine();
                if (i < ((u32)FPolygonHoles.size() - 1) && ImGui::Button("DOWN"))
                {
                    itToErase = polygon;
                    action = 2;
                }
                ImGui::SameLine();
                EDITOR_PROPERTY_VECTOR(glm::vec2, fmt::format("Hole {} points", i + 1), FPolygonHoles[i].data(), false);
                ImGui::PopID();
                ++i;
            }

            if (itToErase != FPolygonHoles.end())
            {
                switch (action)
                {
                case 0:
                {
                    FPolygonHoles.erase(itToErase);
                    break;
                }
                case 1:
                {
                    auto previousIt = itToErase - 1;
                    std::iter_swap(itToErase, previousIt);
                    break;
                }
                case 2:
                {
                    auto nextIt = itToErase + 1;
                    std::iter_swap(itToErase, nextIt);
                    break;
                }
                default:
                    AssertNotReached();
                }
            }
            ImGui::Unindent();
        }

        ImGui::Checkbox("Show extented polygon", &FShowExtentedPolygon);
    }
}

bool SceneActionPolygonalPattern::VirtualDrawInSceneEditor(Rendering::DrawCommandBuffer& parCommandBuffer, Rendering::MaterialInstanceHandle& parMaterial)
{
    parent_type::VirtualDrawInSceneEditor(parCommandBuffer, parMaterial);

    const glm::vec3 offset = (GetSceneItem() == nullptr) ? glm::vec3(0.f) : GetSceneItem()->GetPosition();
    vertices.clear();

    const Polygon2D& polygonToShow = (FShowExtentedPolygon) ? FExtendedPolygon : FPolygon;
    forrange(i, 0, polygonToShow.size())
    {
        const glm::vec2& currentVertex = polygonToShow[i];
        vertices.push_back(glm::vec3(currentVertex.x, 0.f, currentVertex.y) + offset);
    }

    parCommandBuffer.DrawLines(parMaterial, vertices.data(), (u32)vertices.size(), 0xFF00FF00, true);

    if (!FShowExtentedPolygon)
    {
        holesVertices.clear();
        forrange(i, 0, FPolygonHoles.size())
        {
            std::vector<glm::vec3> verts;
            verts.reserve(FPolygonHoles[i].size());

            forrange(j, 0, FPolygonHoles[i].size())
            {
                const glm::vec2& currentVertex = FPolygonHoles[i][j];
                verts.push_back(glm::vec3(currentVertex.x, 0.f, currentVertex.y) + offset);
            }

            holesVertices.push_back(verts);
            parCommandBuffer.DrawLines(parMaterial, holesVertices[holesVertices.size() - 1].data(), (u32)verts.size(), 0xFF0000FF, true);
        }
    }

    foreachitemconst(partition, FPartitions) { parCommandBuffer.DrawLines(parMaterial, partition.data(), (u32)partition.size(), 0.f, 0xFFFF0000, true); }

    return true;
}

void SceneActionPolygonalPattern::ComputeTriangulation()
{
    PolygonPartionner partitionner;
    FPartitions = partitionner.Partition(FPolygon, FPolygonHoles);
}

} // namespace ECSEngine
