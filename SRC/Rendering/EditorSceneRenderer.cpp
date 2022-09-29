#include "stdafx.h"

#include "EditorSceneRenderer.h"

#include "Application/SceneScenario.h"
#include "Common/CameraHelpers.h"
#include "Common/CameraManager.h"
#include "Common/InputManager.h"
#include "Common/IntersectionRoutines.h"
#include "Common/Plane.h"
#include "RenderingCore/BGFXRenderingBackend.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/MeshManager.h"
#include "RenderingCore/RenderPass.h"

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

    FDrawCommandBuffer = Rendering::BGFXRenderingBackend::Instance().CreateCommandBuffer(Rendering::RenderPassId::EDITOR_PASS);
}

void EditorSceneRenderer::Shutdown()
{
    Rendering::BGFXRenderingBackend::Instance().ReleaseCommandBuffer(FDrawCommandBuffer);
    CameraManager::Instance().DestroyCamera(FCameraId);
}

void EditorSceneRenderer::RenderScene(const SceneScenario* parScene)
{

    AssertRelease(FDrawCommandBuffer != nullptr);
    FDrawCommandBuffer->clear();

    FDrawCommandBuffer->SetDebugMarker("Editor rendering");

    const uvec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    const float aspectRatio = Rendering::GLFWDisplayWindowHandler::Instance().AspectRatio();

    bgfx::setViewRect(Rendering::RenderPassId::EDITOR_PASS, 0, 0, windowSize.x, windowSize.y);

    Camera* camera = CameraManager::Instance().GetCamera(FCameraId);
    AssertRelease(camera != nullptr);
    FDrawCommandBuffer->SetViewTranform(camera->GetWorldViewMatrix(), camera->GetProjectionMatrix(aspectRatio));

    AssertRelease(parScene != nullptr);
    const SceneItemsContainer& sceneItems = parScene->GetSceneItems();

    foreachitemconst(sceneItemIt, sceneItems)
    {
        const BaseSceneItem* sceneItem = sceneItemIt.second.get();
        const vec3& position = sceneItem->GetPosition();

        const bool isSelected = sceneItem->ItemSelected();
        const bool isHovered = sceneItem->ItemHovered();

        u32 color = 0xFFFFFFFF;
        if (isSelected)
            color = 0xFF0000FF;
        else if (isHovered)
            color = 0xFFFF00FF;

        FDrawCommandBuffer->DrawAABB(FHandleMaterial, position - vec3(1.0f), position + vec3(1.0f), color);

        if (isSelected)
        {
            vec3 angles = sceneItem->GetEulerAngles();
            mat4 mtx = Translation(position) * EulerAnglesXYZ(angles.x, angles.y, angles.z);

            FDrawCommandBuffer->DrawMesh(FHandleMesh, FHandleMaterial, mtx);
        }
    }

    std::vector<std::shared_ptr<ISceneAction>> sceneActions = parScene->GetSceneActions();
    foreachitem(sceneAction, sceneActions) { sceneAction->DrawInSceneEditor(*FDrawCommandBuffer, FHandleMaterial); }

    bool foundPos = false;
    vec3 mouseWorldPosition = GetWorldPositionFromScreenPosition(*camera, aspectRatio, windowSize, Input::GetMousePosition(), foundPos);

    if (foundPos)
    {
        FDrawCommandBuffer->DrawAABBAsCube(FHandleMaterial, mouseWorldPosition + vec3(-0.1f), mouseWorldPosition + vec3(0.1f), 0xFF00FFFF);
    }

    FDrawCommandBuffer->Submit();
}

} // namespace ECSEngine
