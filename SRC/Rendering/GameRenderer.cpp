#include "stdafx.h"

#include "GameRenderer.h"

#include "Common/CameraManager.h"
#include "Common/Frustum.h"
#include "Common/IFeedbackDrawer.h"
#include "ECSGameplay_Specific/GameplayConstants.h"
#include "ECSGameplay_Specific/GameplayFeedbackDrawer.h"
#include "GFXPickingRenderer.h"
#include "OutlineRenderer.h"
#include "RenderingCore/BGFXRenderingBackend.h"
#include "RenderingCore/Carrier.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/FeedbackParameters.h"
#include "RenderingCore/Framebuffer.h"
#include "RenderingCore/GFXRepresentation.h"
#include "RenderingCore/GFXRepresentationManager.h"
#include "RenderingCore/GFXSelectable.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/MeshCuller.h"
#include "RenderingCore/RenderPass.h"
#include "RenderingCore/VisualModel.h"

namespace ECSEngine
{
namespace Rendering
{

GameRenderer::GameRenderer()
    : Singleton()
{
}

GameRenderer::~GameRenderer()
{
}

void GameRenderer::Initialise()
{
    auto size = GLFWDisplayWindowHandler::Instance().GetSize();

    bgfx::setViewName(RenderPassId::GEOMETRY_PASS, "Geometry pass");
    bgfx::setViewName(RenderPassId::FEEDBACK_PASS, "Feedback pass");
    bgfx::setViewName(RenderPassId::GAME_RENDERER_COMBINE_PASS, "Combine pass");

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

    bgfx::setViewClear(RenderPassId::GEOMETRY_PASS, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x00000000, 1.0f, 0);

    // Feedback pass initialize
    FFeedbackCommandBuffer = Rendering::BGFXRenderingBackend::Instance().CreateCommandBuffer(RenderPassId::FEEDBACK_PASS);

    // Combine pass
    FCombineFramebuffer = new FramebufferInstance(FramebufferSizeType::SCREEN, size);
    FCombineFramebuffer->AddAttachement(false, 1, bgfx::TextureFormat::RGBA8,
          0 | BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    FCombineFramebuffer->InitFramebuffer();

    FCombineCommandBuffer = Rendering::BGFXRenderingBackend::Instance().CreateCommandBuffer(RenderPassId::GAME_RENDERER_COMBINE_PASS);
    bgfx::setViewClear(RenderPassId::GAME_RENDERER_COMBINE_PASS, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x00000000, 1.0f, 0);

    SetViewFramebuffers(size);

    FPickingRenderer.reset(new GFXPickingRenderer());
    FPickingRenderer->Initialise();

    FOutlineRenderer.reset(new OutlineRenderer());
    FOutlineRenderer->Initialise();
}

void GameRenderer::SetViewFramebuffers(const glm::uvec2 parSize)
{
    bgfx::setViewFrameBuffer(RenderPassId::GEOMETRY_PASS, FGeometryFramebuffer->GetHandle());
    bgfx::setViewFrameBuffer(RenderPassId::FEEDBACK_PASS, FGeometryFramebuffer->GetHandle());
    bgfx::setViewFrameBuffer(RenderPassId::GAME_RENDERER_COMBINE_PASS, FCombineFramebuffer->GetHandle());

    bgfx::setViewRect(RenderPassId::GEOMETRY_PASS, 0, 0, parSize.x, parSize.y);
    bgfx::setViewRect(RenderPassId::FEEDBACK_PASS, 0, 0, parSize.x, parSize.y);
    bgfx::setViewRect(RenderPassId::GAME_RENDERER_COMBINE_PASS, 0, 0, parSize.x, parSize.y);
    bgfx::setViewRect(RenderPassId::DEBUG_PASS, 0, 0, parSize.x, parSize.y);
}

void GameRenderer::Shutdown()
{
    FOutlineRenderer->Cleanup();
    FPickingRenderer->Cleanup();

    FGeometryCommandBuffer->clear();
    FFeedbackCommandBuffer->clear();
    FCombineCommandBuffer->clear();

    Rendering::BGFXRenderingBackend::Instance().ReleaseCommandBuffer(FGeometryCommandBuffer);
    Rendering::BGFXRenderingBackend::Instance().ReleaseCommandBuffer(FFeedbackCommandBuffer);
    Rendering::BGFXRenderingBackend::Instance().ReleaseCommandBuffer(FCombineCommandBuffer);

    FGeometryFramebuffer->Destroy();
    delete FGeometryFramebuffer;
    FGeometryFramebuffer = nullptr;

    FCombineFramebuffer->Destroy();
    delete FCombineFramebuffer;
    FCombineFramebuffer = nullptr;
}

void GameRenderer::Render()
{
    SCOPED_PROFILE_CLASS(GameRenderer, Render);

    // Clear command buffers
    FGeometryCommandBuffer->clear();
    FFeedbackCommandBuffer->clear();
    FCombineCommandBuffer->clear();

    // Resize framebuffers
    auto size = GLFWDisplayWindowHandler::Instance().GetSize();
    const bool hasResizedG = FGeometryFramebuffer->ResizeIFN(size);
    const bool hasResizedC = FCombineFramebuffer->ResizeIFN(size);

    if (hasResizedG || hasResizedC)
        SetViewFramebuffers(size);

    bgfx::setViewRect(RenderPassId::GEOMETRY_PASS, 0, 0, size.x, size.y);
    bgfx::setViewRect(RenderPassId::FEEDBACK_PASS, 0, 0, size.x, size.y);
    bgfx::setViewRect(RenderPassId::GAME_UI_PASS, 0, 0, size.x, size.y);
    bgfx::setViewRect(RenderPassId::GAME_RENDERER_COMBINE_PASS, 0, 0, size.x, size.y);
    bgfx::setViewRect(RenderPassId::DEBUG_PASS, 0, 0, size.x, size.y);

    // Gameplay Camera fetch
    const float aspectRatio = GLFWDisplayWindowHandler::Instance().AspectRatio();

    Camera* c = CameraManager::Instance().GetCamera(FGameplayCameraId);
    AssertRelease(c != nullptr);

    Frustum frustum;
    frustum.InitFromCamera(*c, aspectRatio);

    glm::mat4 view = c->GetWorldViewMatrix();
    glm::mat4 proj = c->GetProjectionMatrix(aspectRatio);

    {
        SCOPED_PROFILE(GameRenderer_Render_GeometryPassAndSelection);
        // Geometry pass and selection
        FGeometryCommandBuffer->SetViewTranform(view, proj);
        FPickingRenderer->BeginSelectionPass(FGameplayCameraId);

        std::vector<const GFXRepresentation*> selectedRepresentations;
        std::vector<const GFXRepresentation*> highlightedRepresentations;

        foreachitemconst(gfxRep, GFXRepresentationManager::Instance())
        {
            const Carrier* carrier = gfxRep.second->GetCarrier();
            if (carrier == nullptr)
                continue;

            const VisualModel* visuals = gfxRep.second->GetVisualModel();
            if (visuals == nullptr || !visuals->Visible())
                continue;

            if (Rendering::MeshFrustumCulling::CullMesh(visuals->GetMeshHandle(), carrier->LocalToWorld(), frustum))
            {
                if (visuals->HasMultipassMaterial())
                {
                    if (gfxRep.second->GetPose() != nullptr)
                    {
                        FGeometryCommandBuffer->DrawMeshWithPose(
                              gfxRep.second->GetPose(), visuals->GetMeshHandle(), visuals->GetMultiPassMaterialInstanceHandle(), carrier->LocalToWorld());
                    }
                    else
                    {
                        FGeometryCommandBuffer->DrawMesh(visuals->GetMeshHandle(), visuals->GetMultiPassMaterialInstanceHandle(), carrier->LocalToWorld());
                    }
                }
                else
                {
                    if (gfxRep.second->GetPose() != nullptr)
                    {
                        FGeometryCommandBuffer->DrawMeshWithPose(gfxRep.second->GetPose(), visuals->GetMeshHandle(), visuals->GetMaterialInstanceHandle(), carrier->LocalToWorld());
                    }
                    else
                    {
                        FGeometryCommandBuffer->DrawMesh(visuals->GetMeshHandle(), visuals->GetMaterialInstanceHandle(), carrier->LocalToWorld());
                    }
                }

                FPickingRenderer->PushGFXForSelectionPass(gfxRep.second.get());

                const GFXSelectable* selectable = gfxRep.second->GetSelectable();
                if (selectable != nullptr)
                {
                    if (selectable->IsSelected())
                    {
                        selectedRepresentations.push_back(gfxRep.second.get());
                    }

                    if (selectable->IsHighlighted())
                    {
                        highlightedRepresentations.push_back(gfxRep.second.get());
                    }
                }
            }
        }

        FPickingRenderer->EndSelectionPass();
        FGeometryCommandBuffer->Submit();

        FOutlineRenderer->RenderOutline(
              MemoryView(selectedRepresentations.data(), selectedRepresentations.size()), MemoryView(highlightedRepresentations.data(), highlightedRepresentations.size()));
    }

    // feedback pass
    {
        SCOPED_PROFILE(GameRenderer_Render_FeedbackPass);
        FFeedbackCommandBuffer->SetViewTranform(view, proj);

        GameplayFeedbackDrawer::Instance().DrawFeedback(FFeedbackCommandBuffer);
        FFeedbackCommandBuffer->Submit();
    }

    // combine pass
    {
        // TODO Apply UI AFTER THIS!
        SCOPED_PROFILE(GameRenderer_Render_CombinePass);
        MaterialInstanceHandle combineMaterial = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\combinepass.material");
        MaterialManager::SetSamplerUniform_IKNOWWHATIMDOING("s_GeometryTexture", FGeometryFramebuffer->GetTextureHandle(0).idx, 0);
        MaterialManager::SetSamplerUniform_IKNOWWHATIMDOING("s_FeedbackTexture", FOutlineRenderer->GetTextureHandle(), 1);

        FCombineCommandBuffer->BlitWithMaterial(combineMaterial);
        FCombineCommandBuffer->Submit();
    }
}

u16 GameRenderer::GetFinalTexture() const
{
    return FCombineFramebuffer->GetTextureHandle(0).idx;
}

} // namespace Rendering
} // namespace ECSEngine
