#include "stdafx.h"

#include "OutlineRenderer.h"

#include "Common/CameraManager.h"
#include "Common/ColorUtils.h"
#include "RenderingCore/BGFXRenderingBackend.h"
#include "RenderingCore/Carrier.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/Framebuffer.h"
#include "RenderingCore/GFXRepresentation.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/Skeletton.h"
#include "RenderingCore/VisualModel.h"

namespace ECSEngine
{
namespace Rendering
{
void OutlineRenderer::Initialise()
{
    FDrawBuffer = BGFXRenderingBackend::Instance().CreateCommandBuffer(RenderPassId::OUTLINE_INIT);

    bgfx::setViewName(RenderPassId::OUTLINE_INIT, "Outline init");
    bgfx::setViewClear(RenderPassId::OUTLINE_INIT, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
    bgfx::setViewName(RenderPassId::OUTLINE_HORIZONTAL, "Outline Horizontal");
    bgfx::setViewClear(RenderPassId::OUTLINE_HORIZONTAL, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
    bgfx::setViewName(RenderPassId::OUTLINE_VERTICAL, "Outline Vertical");
    bgfx::setViewClear(RenderPassId::OUTLINE_VERTICAL, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);

    const glm::vec2 windowSize = GLFWDisplayWindowHandler::Instance().GetSize();
    // initial framebuffer
    FInitialFramebuffer = new FramebufferInstance(FramebufferSizeType::SCREEN, windowSize);
    FInitialFramebuffer->AddAttachement(false, 1, bgfx::TextureFormat::RGBA8,
          0 | BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    /*FInitialFramebuffer->AddAttachement(false, 1, bgfx::TextureFormat::D16,
          0 | BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);*/
    FInitialFramebuffer->InitFramebuffer();

    FIntermediaryFramebuffer = new FramebufferInstance(FramebufferSizeType::SCREEN, windowSize);
    FIntermediaryFramebuffer->AddAttachement(false, 1, bgfx::TextureFormat::RGBA8,
          0 | BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    FIntermediaryFramebuffer->InitFramebuffer();

    FDrawIdMaterial = MaterialManager::CreateMaterialInstanceIFN("materials\\circlebuildingpicking.material");
    AssertRelease(FDrawIdMaterial.IsValid());

    FGameplayCameraId = CameraManager::Instance().CreateCameraIFN("GameplayCamera");
    AssertRelease(FGameplayCameraId != -1);
}

void OutlineRenderer::Cleanup()
{
    BGFXRenderingBackend::Instance().ReleaseCommandBuffer(FDrawBuffer);

    FInitialFramebuffer->Destroy();
    delete FInitialFramebuffer;
    FIntermediaryFramebuffer->Destroy();
    delete FIntermediaryFramebuffer;
}

void OutlineRenderer::RenderOutline(MemoryView<const GFXRepresentation*> parSelectedRepresentations, MemoryView<const GFXRepresentation*> parHighlightedRepresentations)
{
    Camera* c = CameraManager::Instance().GetCamera(FGameplayCameraId);
    AssertRelease(c != nullptr);

    const auto windowSize = GLFWDisplayWindowHandler::Instance().GetSize();
    const float aspectRatio = GLFWDisplayWindowHandler::Instance().AspectRatio();
    glm::mat4 view = c->GetWorldViewMatrix();
    glm::mat4 proj = c->GetProjectionMatrix(aspectRatio);

    FInitialFramebuffer->ResizeIFN(windowSize);
    FIntermediaryFramebuffer->ResizeIFN(windowSize);

    bgfx::setViewFrameBuffer(RenderPassId::OUTLINE_INIT, FInitialFramebuffer->GetHandle());
    bgfx::setViewRect(RenderPassId::OUTLINE_INIT, 0, 0, windowSize.x, windowSize.y);
    bgfx::setViewFrameBuffer(RenderPassId::OUTLINE_HORIZONTAL, FIntermediaryFramebuffer->GetHandle());
    bgfx::setViewRect(RenderPassId::OUTLINE_HORIZONTAL, 0, 0, windowSize.x, windowSize.y);
    bgfx::setViewFrameBuffer(RenderPassId::OUTLINE_VERTICAL, FInitialFramebuffer->GetHandle());
    bgfx::setViewRect(RenderPassId::OUTLINE_VERTICAL, 0, 0, windowSize.x, windowSize.y);

    FDrawBuffer->clear();

    FDrawBuffer->SetViewTranform(view, proj);

    foreachitemconst(rep, parSelectedRepresentations) { AddGFXForOutline(rep, true); }
    foreachitemconst(rep, parHighlightedRepresentations) { AddGFXForOutline(rep, false); }

    FDrawBuffer->Submit();
}

void OutlineRenderer::AddGFXForOutline(const GFXRepresentation* parRepresentation, bool parSelected)
{
    const Carrier* carrier = parRepresentation->GetCarrier();
    const VisualModel* visualModel = parRepresentation->GetVisualModel();
    const SkelettonPose* skeletonPose = parRepresentation->GetPose();

    AssertRelease(carrier != nullptr);
    AssertRelease(visualModel != nullptr);

    const u32 colorU32 = parSelected ? 0xFFFFFFFF : 0xFFFF0000;
    const glm::vec4 color = ColorUtils::ConvertToFVEC4(colorU32);
    FDrawBuffer->SetVec4Uniform("u_PickingId", color);
    FDrawBuffer->DrawMesh(visualModel->GetMeshHandle(), FDrawIdMaterial, carrier->LocalToWorld());
}

} // namespace Rendering
} // namespace ECSEngine