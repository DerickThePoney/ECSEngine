#include "stdafx.h"

#include "GameRenderer.h"

#include "Common/CameraManager.h"
#include "Common/Frustum.h"
#include "Common/IFeedbackDrawer.h"
#include "ECSGameplay_Specific/GameplayConstants.h"
#include "ECSGameplay_Specific/GameplayFeedbackDrawer.h"
#include "RenderingCore/BGFXRenderingBackend.h"
#include "RenderingCore/Carrier.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/FeedbackParameters.h"
#include "RenderingCore/Framebuffer.h"
#include "RenderingCore/GFXRepresentationManager.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/MeshCuller.h"
#include "RenderingCore/RenderPass.h"
#include "RenderingCore/VisualModel.h"

namespace ECSEngine
{
namespace Rendering
{

void GameRenderer::Initialise()
{
    auto& size = GLFWDisplayWindowHandler::Instance().GetSize();

    FGameplayCameraId = CameraManager::Instance().CreateCameraIFN("GameplayCamera");
    AssertRelease(FGameplayCameraId != -1);

    // geometry pass initialize
    FGeometryFramebuffer = new FramebufferInstance(FramebufferSizeType::SCREEN, size);
    FGeometryFramebuffer->AddAttachement(false, 1, bgfx::TextureFormat::RGBA8,
          0 | BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    FGeometryFramebuffer->AddAttachement(false, 1, bgfx::TextureFormat::D24S8,
          0 | BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);

    FGeometryFramebuffer->InitFramebuffer();

    FGeometryCommandBuffer = Rendering::BGFXRenderingBackend::Instance().CreateCommandBuffer(RenderPassId::GEOMETRY_PASS);

    bgfx::setViewFrameBuffer(RenderPassId::GEOMETRY_PASS, FGeometryFramebuffer->GetHandle());
    bgfx::setViewClear(RenderPassId::GEOMETRY_PASS, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x00000000, 1.0f, 0);

    // Feedback pass initialize
    FFeedbackFramebuffer = new FramebufferInstance(FramebufferSizeType::SCREEN, size);

    FFeedbackFramebuffer->AddAttachement(false, 1, bgfx::TextureFormat::RGBA8,
          0 | BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);

    FFeedbackFramebuffer->InitFramebuffer();

    FFeedbackCommandBuffer = Rendering::BGFXRenderingBackend::Instance().CreateCommandBuffer(RenderPassId::FEEDBACK_PASS);
    bgfx::setViewFrameBuffer(RenderPassId::FEEDBACK_PASS, FFeedbackFramebuffer->GetHandle());
    bgfx::setViewClear(RenderPassId::FEEDBACK_PASS, BGFX_CLEAR_COLOR, 0x00000000);

    // Combine pass
    FCombineCommandBuffer = Rendering::BGFXRenderingBackend::Instance().CreateCommandBuffer(RenderPassId::COMBINE_PASS);
    bgfx::setViewClear(RenderPassId::COMBINE_PASS, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x00000000, 1.0f, 0);

    bgfx::setViewRect(RenderPassId::GEOMETRY_PASS, 0, 0, size.x, size.y);
    bgfx::setViewRect(RenderPassId::FEEDBACK_PASS, 0, 0, size.x, size.y);
    bgfx::setViewRect(RenderPassId::COMBINE_PASS, 0, 0, size.x, size.y);
    bgfx::setViewRect(RenderPassId::DEBUG_PASS, 0, 0, size.x, size.y);
}

void GameRenderer::Shutdown()
{
    FGeometryCommandBuffer->clear();
    FFeedbackCommandBuffer->clear();
    FCombineCommandBuffer->clear();

    Rendering::BGFXRenderingBackend::Instance().ReleaseCommandBuffer(FGeometryCommandBuffer);
    Rendering::BGFXRenderingBackend::Instance().ReleaseCommandBuffer(FFeedbackCommandBuffer);
    Rendering::BGFXRenderingBackend::Instance().ReleaseCommandBuffer(FCombineCommandBuffer);

    delete FGeometryFramebuffer;
    FGeometryFramebuffer = nullptr;
    delete FFeedbackFramebuffer;
    FFeedbackFramebuffer = nullptr;
}

void GameRenderer::Render()
{
    // Clear command buffers
    FGeometryCommandBuffer->clear();
    FFeedbackCommandBuffer->clear();
    FCombineCommandBuffer->clear();

    // Resize framebuffers
    auto& size = GLFWDisplayWindowHandler::Instance().GetSize();
    FGeometryFramebuffer->ResizeIFN(size);
    FFeedbackFramebuffer->ResizeIFN(size);

    // Gameplay Camera fetch
    const float aspectRatio = GLFWDisplayWindowHandler::Instance().AspectRatio();

    Camera* c = CameraManager::Instance().GetCamera(FGameplayCameraId);
    AssertRelease(c != nullptr);

    Frustum frustum;
    frustum.InitFromCamera(*c, aspectRatio);

    glm::mat4 view = c->GetWorldViewMatrix();
    glm::mat4 proj = c->GetProjectionMatrix(aspectRatio);

    // Geometry pass
    FGeometryCommandBuffer->SetViewTranform(view, proj);

    foreachitemconst(gfxRep, GFXRepresentationManager::Instance())
    {
        const Carrier* carrier = gfxRep.second->GetCarrier();
        if (carrier == nullptr)
            continue;

        const VisualModel* visuals = gfxRep.second->GetVisualModel();
        if (visuals == nullptr)
            continue;

        if (Rendering::MeshFrustumCulling::CullMesh(visuals->GetMeshHandle(), carrier->LocalToWorld(), frustum))
        {
            FGeometryCommandBuffer->DrawMesh(visuals->GetMeshHandle(), visuals->GetMaterialInstanceHandle(), carrier->LocalToWorld());
        }
    }

    FGeometryCommandBuffer->Submit();

    // feedback pass
    FFeedbackCommandBuffer->SetViewTranform(view, proj);

    GameplayFeedbackDrawer::Instance().DrawFeedback(FFeedbackCommandBuffer);
    FFeedbackCommandBuffer->Submit();

    // combine pass
    MaterialInstanceHandle combineMaterial = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\combinepass.material");
    MaterialManager::SetSamplerUniform_IKNOWWHATIMDOING("s_GeometryTexture", FGeometryFramebuffer->GetTextureHandle(0).idx, 0);
    MaterialManager::SetSamplerUniform_IKNOWWHATIMDOING("s_FeedbackTexture", FFeedbackFramebuffer->GetTextureHandle(0).idx, 1);

    FCombineCommandBuffer->BlitWithMaterial(combineMaterial);
    FCombineCommandBuffer->Submit();
}

} // namespace Rendering
} // namespace ECSEngine