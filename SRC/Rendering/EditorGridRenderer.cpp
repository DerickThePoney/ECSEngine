#include "stdafx.h"

#include "EditorGridRenderer.h"

#include "Common/CameraManager.h"
#include "RenderingCore/BGFXRenderingBackend.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/RenderingState.h"

namespace ECSEngine
{
EditorGridRenderer::EditorGridRenderer()
{
}

EditorGridRenderer::~EditorGridRenderer()
{
}

void EditorGridRenderer::Initialise()
{
    FDrawCommandBuffer = Rendering::BGFXRenderingBackend::Instance().CreateCommandBuffer(Rendering::RenderPassId::EDITOR_PASS);
    FGridMaterial = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\editorgridmaterial.material");
    AssertRelease(FGridMaterial.IsValid());
}

void EditorGridRenderer::Shutdown()
{
    Rendering::BGFXRenderingBackend::Instance().ReleaseCommandBuffer(FDrawCommandBuffer);
}

void EditorGridRenderer::RenderScene()
{
    AssertRelease(FDrawCommandBuffer != nullptr);

    FDrawCommandBuffer->clear();

    u32 cameraId = CameraManager::Instance().CreateCameraIFN("EditorCamera");
    Camera* c = CameraManager::Instance().GetCamera(cameraId);
    AssertRelease(c);

    const float aspectRatio = Rendering::GLFWDisplayWindowHandler::Instance().AspectRatio();
    FDrawCommandBuffer->SetViewTranform(c->GetWorldViewMatrix(), c->GetProjectionMatrix(aspectRatio));

    Rendering::RenderingState state;
    state.PartiallyModifyState(BGFX_STATE_BLEND_ALPHA);
    state.ApplyState();

    const std::vector<glm::vec3> vertices = { glm::vec3(-1, -1, 0), glm::vec3(1, -1, 0), glm::vec3(1, 1, 0), glm::vec3(-1, 1, 0) };
    const u16 indices[] = { 0, 1, 2, 0, 2, 3 };

    Rendering::MaterialManager::SetVec4Uniform("u_nearFar", glm::vec4(c->Near(), c->Far(), 0.0, 0.0));

    FDrawCommandBuffer->DrawVertices(vertices.data(), (u32)vertices.size(), indices, 6, FGridMaterial);

    FDrawCommandBuffer->Submit();
}

} // namespace ECSEngine
