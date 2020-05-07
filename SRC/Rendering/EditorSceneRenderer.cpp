#include "stdafx.h"

#include "EditorSceneRenderer.h"

#include "Application/SceneScenario.h"
#include "Common/CameraManager.h"
#include "RenderingCore/BGFXRenderer.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/MeshManager.h"

namespace ECSEngine
{

EditorSceneRenderer::EditorSceneRenderer()
{
}

EditorSceneRenderer::~EditorSceneRenderer()
{
}

void EditorSceneRenderer::Initialise(const std::string& parHandleFileName, const std::string& parHandleMaterial)
{
    FHandleMesh = Rendering::MeshManager::Instance().CreateMesh(parHandleFileName);
    AssertRelease(FHandleMesh.IsValid());
    FHandleMaterial = Rendering::MaterialManager::CreateMaterialInstanceIFN(parHandleMaterial);
    AssertRelease(FHandleMaterial.IsValid());

    FCameraId = CameraManager::Instance().CreateCameraIFN("EditorCamera");

    FDrawCommandBuffer = Rendering::BGFXRenderer::Instance().CreateCommandBuffer(Rendering::RenderPassId::GEOMETRY_PASS);
}

void EditorSceneRenderer::Shutdown()
{
    Rendering::BGFXRenderer::Instance().ReleaseCommandBuffer(FDrawCommandBuffer);
    CameraManager::Instance().DestroyCamera(FCameraId);
}

void EditorSceneRenderer::RenderScene(const SceneScenario* parScene)
{
    AssertRelease(FDrawCommandBuffer != nullptr);
    FDrawCommandBuffer->clear();

    const glm::uvec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();

    Camera* camera = CameraManager::Instance().GetCamera(FCameraId);
    AssertRelease(camera != nullptr);
    FDrawCommandBuffer->SetViewTranform(camera->GetWorldViewMatrix(), camera->GetProjectionMatrix(float(windowSize.x) / float(windowSize.y)));

    AssertRelease(parScene != nullptr);
    const std::vector<std::shared_ptr<BaseSceneItem>>& sceneItems = parScene->GetSceneItems();

    foreachitemconst(sceneItem, sceneItems)
    {
        const glm::vec3& position = sceneItem->GetPosition();

        const bool isSelected = sceneItem->ItemSelected();
        const bool isHovered = sceneItem->ItemHovered();

        u32 color = 0xFFFFFFFF;
        if (isSelected)
            color = 0xFF0000FF;
        else if (isHovered)
            color = 0xFFFF00FF;

        FDrawCommandBuffer->DrawAABB(FHandleMaterial, position - glm::vec3(1.0f), position + glm::vec3(1.0f), color);

        if (isSelected)
        {
            glm::vec3 angles = sceneItem->GetEulerAngles();
            glm::mat4 mtx = glm::translate(position) * glm::eulerAngleXYZ(angles.x, angles.y, angles.z);

            FDrawCommandBuffer->DrawMesh(FHandleMesh, FHandleMaterial, mtx);
        }
    }

    std::vector<std::shared_ptr<ISceneAction>> sceneActions = parScene->GetSceneActions();
    foreachitem(sceneAction, sceneActions) { sceneAction->DrawInSceneEditor(*FDrawCommandBuffer, FHandleMaterial); }

    FDrawCommandBuffer->Submit();
}

} // namespace ECSEngine