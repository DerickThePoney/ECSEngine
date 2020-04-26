#include "stdafx.h"

#include "SceneObjectsPickingRenderer.h"

#include "Application/Scene.h"
#include "Application/SceneItems.h"
#include "Common/CameraManager.h"
#include "Common/InputManager.h"
#include "RenderingCore/BGFXRenderer.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/MaterialManager.h"

namespace ECSEngine
{

SceneObjectsPickingRenderer::SceneObjectsPickingRenderer()
    : FReadingAvailable(false)
{
}

SceneObjectsPickingRenderer::~SceneObjectsPickingRenderer()
{
}

bgfx::TextureHandle FPickingTexture;
bgfx::TextureHandle FPickingDepthTexture;
bgfx::TextureHandle FPickingBlitTexture;

bgfx::FrameBufferHandle FPickingFramebuffer;

void SceneObjectsPickingRenderer::Initialise()
{
    bgfx::setViewName(Rendering::RenderPassId::SELECTION_PASS, "Picking pass");
    bgfx::setViewClear(Rendering::RenderPassId::SELECTION_PASS, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 0.0f, 0);

    FDrawCommandBuffer = Rendering::BGFXRenderer::Instance().CreateCommandBuffer(Rendering::RenderPassId::SELECTION_PASS);
    FBlitCommandBuffer = Rendering::BGFXRenderer::Instance().CreateCommandBuffer(Rendering::RenderPassId::SELECTION_BLIT_PASS);

    FPickingTexture = bgfx::createTexture2D(PickTextureSize, PickTextureSize, false, 1, bgfx::TextureFormat::RGBA8,
          0 | BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);

    FPickingDepthTexture = bgfx::createTexture2D(PickTextureSize, PickTextureSize, false, 1, bgfx::TextureFormat::D24S8,
          0 | BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);

    FPickingBlitTexture = bgfx::createTexture2D(PickTextureSize, PickTextureSize, false, 1, bgfx::TextureFormat::RGBA8,
          0 | BGFX_TEXTURE_BLIT_DST | BGFX_TEXTURE_READ_BACK | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP |
                BGFX_SAMPLER_V_CLAMP);

    bgfx::TextureHandle rt[2] = { FPickingTexture, FPickingDepthTexture };
    FPickingFramebuffer = bgfx::createFrameBuffer(2, rt, true);

    FDrawIdMaterial = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\vertexcolormaterial.material");
    AssertRelease(FDrawIdMaterial.IsValid());
}

void SceneObjectsPickingRenderer::Shutdown()
{
    Rendering::BGFXRenderer::Instance().ReleaseCommandBuffer(FBlitCommandBuffer);
    Rendering::BGFXRenderer::Instance().ReleaseCommandBuffer(FDrawCommandBuffer);

    bgfx::destroy(FPickingFramebuffer);
    bgfx::destroy(FPickingBlitTexture);
    bgfx::destroy(FPickingDepthTexture);
    bgfx::destroy(FPickingTexture);
}

void SceneObjectsPickingRenderer::RenderScene(const Scene* parScene)
{
    AssertRelease(FDrawCommandBuffer != nullptr);
    AssertRelease(FBlitCommandBuffer != nullptr);

    FDrawCommandBuffer->clear();
    FBlitCommandBuffer->clear();

    u32 cameraId = CameraManager::Instance().CreateCameraIFN("EditorCamera");
    Camera* c = CameraManager::Instance().GetCamera(cameraId);
    AssertRelease(c);

    bgfx::setViewFrameBuffer(Rendering::RenderPassId::SELECTION_PASS, FPickingFramebuffer);
    bgfx::setViewRect(Rendering::RenderPassId::SELECTION_PASS, 0, 0, PickTextureSize, PickTextureSize);

    const glm::uvec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    const glm::mat4 invProjectionMatrix = glm::inverse(c->GetProjectionMatrix((float)windowSize.x / (float)windowSize.y));
    const glm::mat4 viewWorldMatrix = glm::inverse(c->GetWorldViewMatrix());

    const glm::vec2 mousePosition = Input::GetMousePosition();
    float mouseXNDC = (mousePosition.x / (float)windowSize.x) * 2.0f - 1.0f;
    float mouseYNDC = ((windowSize.y - mousePosition.y) / (float)windowSize.y) * 2.0f - 1.0f;

    const glm::vec4 pickEyeH = invProjectionMatrix * glm::vec4(mouseXNDC, mouseYNDC, 1.0f, 1.0f);
    const glm::vec4 pickAtH = invProjectionMatrix * glm::vec4(mouseXNDC, mouseYNDC, 0.0f, 1.0f);
    const glm::vec3 pickEye = viewWorldMatrix * pickEyeH / pickEyeH.w;
    const glm::vec3 pickAt = viewWorldMatrix * pickAtH / pickAtH.w;

    const glm::mat4 pickView = glm::lookAt(pickEye, pickAt, glm::vec3(0.0f, 0.0f, 1.0f));
    const glm::mat4 pickProj = glm::perspective(glm::radians(3.0f), 1.0f, 0.1f, 500.0f);

    bgfx::setViewTransform(Rendering::RenderPassId::SELECTION_PASS, &pickView[0][0], &pickProj[0][0]);

    AssertRelease(parScene != nullptr);
    const std::vector<std::shared_ptr<BaseSceneItem>>& sceneItems = parScene->GetSceneItems();

    foreachitemconst(sceneItem, sceneItems)
    {
        const glm::vec3& position = sceneItem->GetPosition();
        const u32 sceneItemID = sceneItem->Id() + 1;
        const u32 color = 0xFF000000 | (sceneItemID & 0x00FFFFFF);
        FDrawCommandBuffer->DrawAABBAsCube(FDrawIdMaterial, position - glm::vec3(1.0f), position + glm::vec3(1.0f), color);
    }

    FDrawCommandBuffer->Submit();

    // Blit and read
    bgfx::blit(Rendering::RenderPassId::SELECTION_BLIT_PASS, FPickingBlitTexture, 0, 0, FPickingTexture);
    u32 availableAtFrame = bgfx::readTexture(FPickingBlitTexture, FSelectionData);
    if (!FReadingAvailable)
        Rendering::BGFXRenderer::Instance().AddRequestOnSpecificFrame(availableAtFrame, DELEGATE(&SceneObjectsPickingRenderer::SetDataIsAvailable, *this));
}

void SceneObjectsPickingRenderer::DrawDebugData(bool* parOpen)
{
    const glm::uvec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    ImGui::SetNextWindowPos(ImVec2(windowSize.x - windowSize.x / 5.0f - 10.0f, 10.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(windowSize.x / 5.0f, windowSize.y / 2.0f), ImGuiCond_FirstUseEver);
    ImGui::Begin("Picking texture", parOpen, 0);

    ImGui::Image(FPickingTexture, ImVec2(windowSize.x / 5.0f - 16.0f, windowSize.x / 5.0f - 16.0f));

    if (FReadingAvailable)
    {
        std::map<u32, u32> mapIndexToNbHits;
        for (u32 i = 0; i < PickTextureSize * PickTextureSize * 4; i += 4)
        {
            u32 index = (u32)FSelectionData[i] + ((u32)FSelectionData[i + 1] << 8) + ((u32)FSelectionData[i + 2] << 16);
            if (index > 0)
            {
                if (mapIndexToNbHits.find(index - 1) != mapIndexToNbHits.end())
                    mapIndexToNbHits[index - 1] += 1;
                else
                    mapIndexToNbHits[index - 1] = 1;
            }
        }

        u32 nbHitsMax = 0;
        u32 indexMaxHits = -1;
        foreachitemconst(it, mapIndexToNbHits)
        {
            if (it.second > nbHitsMax)
            {
                nbHitsMax = it.second;
                indexMaxHits = it.first;
            }
        }

        if (indexMaxHits != -1)
        {
            ImGui::Text("Scene item with max hits: %d", indexMaxHits);
            ImGui::Text("Nb hits: %d", nbHitsMax);
        }
        else
        {
            ImGui::Text("No hits");
        }
    }

    ImGui::End();
}

void SceneObjectsPickingRenderer::SetDataIsAvailable()
{
    FReadingAvailable = true;
}

} // namespace ECSEngine