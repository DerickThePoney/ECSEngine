#include "stdafx.h"

#include "SceneObjectsPickingRenderer.h"

#include "Application/PropertyDrawer.h"
#include "Application/SceneItems.h"
#include "Application/SceneScenario.h"
#include "Common/CameraHelpers.h"
#include "Common/CameraManager.h"
#include "Common/InputManager.h"
#include "Common/Ray.h"
#include "RenderingCore/BGFXRenderer.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/Framebuffer.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/RenderingState.h"

namespace ECSEngine
{

SceneObjectsPickingRenderer::SceneObjectsPickingRenderer()
    : FReadingAvailable(false)
    , FSelectedSceneItem(-1)
    , FSelectedSceneItemHits(0)
    , FSelectionFoV(1.0f)
{
}

SceneObjectsPickingRenderer::~SceneObjectsPickingRenderer()
{
}

bgfx::TextureHandle FPickingBlitTexture;

void SceneObjectsPickingRenderer::Initialise()
{
    bgfx::setViewName(Rendering::RenderPassId::SELECTION_PASS, "Picking pass");
    bgfx::setViewClear(Rendering::RenderPassId::SELECTION_PASS, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);

    FDrawCommandBuffer = Rendering::BGFXRenderer::Instance().CreateCommandBuffer(Rendering::RenderPassId::SELECTION_PASS);

    FPickFramebuffer = new Rendering::FramebufferInstance(Rendering::FramebufferSizeType::CUSTOM, glm::uvec2(PickTextureSize, PickTextureSize));
    FPickFramebuffer->AddAttachement(false, 1, bgfx::TextureFormat::RGBA8,
          0 | BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    FPickFramebuffer->AddAttachement(false, 1, bgfx::TextureFormat::D24S8,
          0 | BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);

    FPickingBlitTexture = bgfx::createTexture2D(PickTextureSize, PickTextureSize, false, 1, bgfx::TextureFormat::RGBA8,
          0 | BGFX_TEXTURE_BLIT_DST | BGFX_TEXTURE_READ_BACK | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP |
                BGFX_SAMPLER_V_CLAMP);

    FPickFramebuffer->InitFramebuffer();

    FDrawIdMaterial = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\objectpickingmaterial.material");
    AssertRelease(FDrawIdMaterial.IsValid());
}

void SceneObjectsPickingRenderer::Shutdown()
{
    Rendering::BGFXRenderer::Instance().ReleaseCommandBuffer(FDrawCommandBuffer);

    FPickFramebuffer->Destroy();
    delete FPickFramebuffer;
    bgfx::destroy(FPickingBlitTexture);
}

void SceneObjectsPickingRenderer::RenderScene(const SceneScenario* parScene)
{
    AssertRelease(FDrawCommandBuffer != nullptr);

    FDrawCommandBuffer->clear();

    u32 cameraId = CameraManager::Instance().CreateCameraIFN("EditorCamera");
    Camera* c = CameraManager::Instance().GetCamera(cameraId);
    AssertRelease(c);

    bgfx::setViewFrameBuffer(Rendering::RenderPassId::SELECTION_PASS, FPickFramebuffer->GetHandle());
    bgfx::setViewRect(Rendering::RenderPassId::SELECTION_PASS, 0, 0, PickTextureSize, PickTextureSize);

    const glm::uvec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    const float aspectRatio = Rendering::GLFWDisplayWindowHandler::Instance().AspectRatio();
    Ray ray = GetCameraRayFromMouseInput(*c, aspectRatio, windowSize, Input::GetMousePosition());

    const glm::mat4 pickView = glm::lookAt(ray.FOrigin, ray.FOrigin + ray.FDirection, glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 pickProj = glm::perspective(glm::radians(FSelectionFoV), 1.0f, 0.1f, 500.0f);

    FDrawCommandBuffer->SetViewTranform(pickView, pickProj);

    AssertRelease(parScene != nullptr);
    const SceneItemsContainer& sceneItems = parScene->GetSceneItems();

    foreachitemconst(sceneItem, sceneItems)
    {
        const glm::vec3& position = sceneItem.second->GetPosition();
        const u32 sceneItemID = (sceneItem.second->Id() + 1);
        const u32 color = 0xFF000000 | (sceneItemID & 0x00FFFFFF);
        FDrawCommandBuffer->DrawAABBAsCube(FDrawIdMaterial, position - glm::vec3(1.0f), position + glm::vec3(1.0f), color);
    }

    FDrawCommandBuffer->Submit();

    // Blit and read
    bgfx::blit(Rendering::RenderPassId::SELECTION_BLIT_PASS, FPickingBlitTexture, 0, 0, FPickFramebuffer->GetTextureHandle(0));
    u32 availableAtFrame = bgfx::readTexture(FPickingBlitTexture, FSelectionData);
    if (!FReadingAvailable)
        Rendering::BGFXRenderer::Instance().AddRequestOnSpecificFrame(availableAtFrame, DELEGATE(&SceneObjectsPickingRenderer::SetDataIsAvailable, *this));

    if (FReadingAvailable)
    {
        std::map<u32, u32> mapIndexToNbHits;
        for (u32 i = 0; i < PickTextureSize * PickTextureSize * 4; i += 4)
        {
            u32 index = ((u32)FSelectionData[i] + ((u32)FSelectionData[i + 1] << 8) + ((u32)FSelectionData[i + 2] << 16));
            if (index > 0)
            {
                if (mapIndexToNbHits.find(index - 1) != mapIndexToNbHits.end())
                    mapIndexToNbHits[index - 1] += 1;
                else
                    mapIndexToNbHits[index - 1] = 1;
            }
        }

        FSelectedSceneItemHits = 0;
        FSelectedSceneItem = -1;
        foreachitemconst(it, mapIndexToNbHits)
        {
            if (it.second > FSelectedSceneItemHits)
            {
                FSelectedSceneItemHits = it.second;
                FSelectedSceneItem = it.first;
            }
        }
    }
}

void SceneObjectsPickingRenderer::DrawDebugData(bool* parOpen)
{
    const glm::uvec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    ImGui::SetNextWindowPos(ImVec2(windowSize.x - windowSize.x / 5.0f - 10.0f, 10.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(windowSize.x / 5.0f, windowSize.y / 2.0f), ImGuiCond_FirstUseEver);
    ImGui::Begin("Picking texture", parOpen, 0);

    /*ImGui::Image(FPickingTexture, ImVec2(windowSize.x / 5.0f - 16.0f, windowSize.x / 5.0f - 16.0f));

    EDITOR_PROPERTY_WITH_LIMITS("Selection FoV", FSelectionFoV, 0.f, 180.f);

    if (FSelectedSceneItem != -1)
    {
        ImGui::Text("Scene item with max hits: %d", FSelectedSceneItem);
        ImGui::Text("Nb hits: %d", FSelectedSceneItemHits);
    }
    else
    {
        ImGui::Text("No hits");
    }*/

    ImGui::End();
}

std::pair<u32, u32> SceneObjectsPickingRenderer::GetPickedItemAndHits(const float minProportion /*= 0.0f*/) const
{
    if (minProportion > 0.0f && FSelectedSceneItem != -1 && FSelectedSceneItemHits / (PickTextureSize * PickTextureSize) > minProportion)
    {
        return { FSelectedSceneItem, FSelectedSceneItemHits };
    }
    return { FSelectedSceneItem, 0 };
}

void SceneObjectsPickingRenderer::SetDataIsAvailable()
{
    FReadingAvailable = true;
}

} // namespace ECSEngine