#include "stdafx.h"

#include "GFXPickingRenderer.h"

#include "Common/CameraHelpers.h"
#include "Common/CameraManager.h"
#include "Common/ColorUtils.h"
#include "Common/InputManager.h"
#include "Common/Ray.h"
#include "ECSCore/AdjustableDebugParameters.h"
#include "RenderingCore/BGFXRenderingBackend.h"
#include "RenderingCore/Carrier.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/Framebuffer.h"
#include "RenderingCore/GFXRepresentation.h"
#include "RenderingCore/GFXRepresentationManager.h"
#include "RenderingCore/GFXSelectable.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/RenderingState.h"
#include "RenderingCore/Skeletton.h"
#include "RenderingCore/VisualModel.h"

namespace ECSEngine
{
namespace Rendering
{

bgfx::TextureHandle FPickingBlitTexture;
void GFXPickingRenderer::Initialise()
{
    bgfx::setViewName(Rendering::RenderPassId::SELECTION_PASS, "Picking pass");
    bgfx::setViewName(Rendering::RenderPassId::SELECTION_BLIT_PASS, "Picking blit pass");
    bgfx::setViewClear(Rendering::RenderPassId::SELECTION_PASS, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);

    FDrawCommandBuffer = Rendering::BGFXRenderingBackend::Instance().CreateCommandBuffer(Rendering::RenderPassId::SELECTION_PASS);

    FPickFramebuffer = new Rendering::FramebufferInstance(Rendering::FramebufferSizeType::CUSTOM, glm::uvec2(PickTextureSize, PickTextureSize));
    FPickFramebuffer->AddAttachement(false, 1, bgfx::TextureFormat::RGBA8,
          0 | BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    FPickFramebuffer->AddAttachement(false, 1, bgfx::TextureFormat::D24S8,
          0 | BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);

    FPickingBlitTexture = bgfx::createTexture2D(PickTextureSize, PickTextureSize, false, 1, bgfx::TextureFormat::RGBA8,
          0 | BGFX_TEXTURE_BLIT_DST | BGFX_TEXTURE_READ_BACK | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP |
                BGFX_SAMPLER_V_CLAMP);

    FPickFramebuffer->InitFramebuffer();

    FDrawIdMaterial = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\circlebuildingpicking.material");
    AssertRelease(FDrawIdMaterial.IsValid());
}

void GFXPickingRenderer::Cleanup()
{
    Rendering::BGFXRenderingBackend::Instance().ReleaseCommandBuffer(FDrawCommandBuffer);

    FPickFramebuffer->Destroy();
    delete FPickFramebuffer;
    bgfx::destroy(FPickingBlitTexture);
}

void GFXPickingRenderer::BeginSelectionPass(const u32 parCamera)
{
    AssertRelease(FDrawCommandBuffer != nullptr);

    FDrawCommandBuffer->clear();

    Camera* c = CameraManager::Instance().GetCamera(parCamera);
    AssertRelease(c);

    bgfx::setViewFrameBuffer(Rendering::RenderPassId::SELECTION_PASS, FPickFramebuffer->GetHandle());
    bgfx::setViewRect(Rendering::RenderPassId::SELECTION_PASS, 0, 0, PickTextureSize, PickTextureSize);

    const glm::uvec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    const float aspectRatio = Rendering::GLFWDisplayWindowHandler::Instance().AspectRatio();
    Ray ray = GetCameraRayFromMouseInput(*c, aspectRatio, windowSize, Input::GetMousePosition());

    const glm::mat4 pickView = glm::lookAt(ray.FOrigin, ray.FOrigin + ray.FDirection, glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 pickProj = glm::perspective(glm::radians(FSelectionFoV), 1.0f, c->Near(), c->Far());

    FDrawCommandBuffer->SetViewTranform(pickView, pickProj);
}

void GFXPickingRenderer::PushGFXForSelectionPass(const GFXRepresentation* parGFX)
{
    const GFXSelectable* selectable = parGFX->GetSelectable();
    if (selectable == nullptr)
        return;

    if (!selectable->Selectable())
        return;

    const Carrier* carrier = parGFX->GetCarrier();
    const VisualModel* visualModel = parGFX->GetVisualModel();
    const SkelettonPose* skeletonPose = parGFX->GetPose();

    AssertRelease(carrier != nullptr);
    AssertRelease(visualModel != nullptr);

    const u32 gfxId = (parGFX->Id() + 1);
    const u32 id = 0xFF000000 | (gfxId & 0x00FFFFFF);
    const glm::vec4 color = ColorUtils::ConvertToFVEC4(id);
    FDrawCommandBuffer->SetVec4Uniform("u_PickingId", color);
    if (visualModel->HasMultipassMaterial())
        FDrawCommandBuffer->DrawMesh(visualModel->GetMeshHandle(), visualModel->GetMultiPassMaterialInstanceHandle(), carrier->LocalToWorld());
    else
        FDrawCommandBuffer->DrawMesh(visualModel->GetMeshHandle(), FDrawIdMaterial, carrier->LocalToWorld());
}

void GFXPickingRenderer::EndSelectionPass()
{
    FDrawCommandBuffer->Submit();

    // Blit and read
    bgfx::blit(Rendering::RenderPassId::SELECTION_BLIT_PASS, FPickingBlitTexture, 0, 0, FPickFramebuffer->GetTextureHandle(0));
    u32 availableAtFrame = bgfx::readTexture(FPickingBlitTexture, FSelectionData);
    if (!FReadingAvailable)
        Rendering::BGFXRenderingBackend::Instance().AddRequestOnSpecificFrame(availableAtFrame, DELEGATE(&GFXPickingRenderer::SetDataIsAvailable, *this));

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

        FHighlithedGFXIdHits = 0;
        FPreviousFrameHighlightedGFXId = FHighlightedGFXId;
        FHighlightedGFXId = -1;
        foreachitemconst(it, mapIndexToNbHits)
        {
            if (it.second > FHighlithedGFXIdHits)
            {
                FHighlithedGFXIdHits = it.second;
                FHighlightedGFXId = it.first;
            }
        }

        ADJUSTABLE_DEBUG_PARAMETER_BOOLEAN(debugSelection, false, "Debug selection", "Selection");
        if (debugSelection && FHighlightedGFXId != -1)
            std::cout << FHighlightedGFXId << "\t" << FHighlithedGFXIdHits << "\n";

        FPreviousFrameSelectedGFXId = FSelectedGFXId;

        if (FHighlightedGFXId != -1 && Input::GetMouseButtonState(MouseButtons::MOUSE_BUTTON_1) && Input::GetMouseButtonHasChanged(MouseButtons::MOUSE_BUTTON_1))
        {
            FSelectedGFXId = FHighlightedGFXId;
        }
        else if (FSelectedGFXId != -1 && Input::GetMouseButtonState(MouseButtons::MOUSE_BUTTON_2) && Input::GetMouseButtonHasChanged(MouseButtons::MOUSE_BUTTON_2))
        {
            FSelectedGFXId = -1;
        }

        if (FPreviousFrameHighlightedGFXId != FHighlightedGFXId)
        {
            GFXRepresentation* rep = GFXRepresentationManager::Instance().GetGFX(FPreviousFrameHighlightedGFXId);
            if (rep != nullptr)
            {
                rep->GetSelectable()->SetHighlighted(false);
            }

            rep = GFXRepresentationManager::Instance().GetGFX(FHighlightedGFXId);
            if (rep != nullptr)
            {
                rep->GetSelectable()->SetHighlighted(true);
            }
        }

        if (FPreviousFrameSelectedGFXId != FSelectedGFXId)
        {
            GFXRepresentation* rep = GFXRepresentationManager::Instance().GetGFX(FPreviousFrameSelectedGFXId);
            if (rep != nullptr)
            {
                rep->GetSelectable()->SetSelected(false);
            }

            rep = GFXRepresentationManager::Instance().GetGFX(FSelectedGFXId);
            if (rep != nullptr)
            {
                rep->GetSelectable()->SetSelected(true);
            }
        }
    }
}

// Outline rendering: frame size / 4 - First pass, drawing - second pass horizontal filter - third pass vertical filter (use stencil?) - then upcale to frame resolution via
// multiple blitting - blit onto the final frame in the combine pass

void GFXPickingRenderer::SetDataIsAvailable()
{
    FReadingAvailable = true;
}

} // namespace Rendering
} // namespace ECSEngine