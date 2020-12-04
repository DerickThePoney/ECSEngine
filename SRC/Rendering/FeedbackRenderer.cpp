#include "stdafx.h"

#include "FeedbackRenderer.h"

#include "Common/CameraManager.h"
#include "ECSGameplay_Specific/GameplayConstants.h"
#include "RenderingCore/BGFXRenderer.h"
#include "RenderingCore/FeedbackParameters.h"
#include "RenderingCore/Framebuffer.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/MaterialManager.h"

namespace ECSEngine
{
namespace Rendering
{

FeedbackRenderer::FeedbackRenderer()
{
}

FeedbackRenderer::~FeedbackRenderer()
{
}
// bgfx::TextureHandle FFeedbackTexture;
// bgfx::FrameBufferHandle FFeedbackFrameBuffer;
void FeedbackRenderer::Initialise()
{
    auto& size = GLFWDisplayWindowHandler::Instance().GetSize();
    FFramebuffer = new FramebufferInstance(FramebufferSizeType::SCREEN, size);

    FFramebuffer->AddAttachement(false, 1, bgfx::TextureFormat::RGBA8,
          0 | BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);

    /*FFeedbackTexture = bgfx::createTexture2D(size.x, size.y, false, 1, bgfx::TextureFormat::RGBA8,
          0 | BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);

    bgfx::TextureHandle rt[1] = { FFeedbackTexture };
    FFeedbackFrameBuffer = bgfx::createFrameBuffer(1, rt, true);*/

    FFramebuffer->InitFramebuffer();

    FDrawCommandBuffer = Rendering::BGFXRenderer::Instance().CreateCommandBuffer(RenderPassId::FEEDBACK_PASS);
}

void FeedbackRenderer::Shutdown()
{
    Rendering::BGFXRenderer::Instance().ReleaseCommandBuffer(FDrawCommandBuffer);

    FFramebuffer->Destroy();
    delete FFramebuffer;
}

void FeedbackRenderer::Render()
{
    AssertRelease(FDrawCommandBuffer != nullptr);

    FDrawCommandBuffer->clear();

    FFramebuffer->ResizeIFN(GLFWDisplayWindowHandler::Instance().GetSize());

    // TODO move all this somewhere else
    u32 cameraId = CameraManager::Instance().CreateCameraIFN("EditorCamera");
    Camera* c = CameraManager::Instance().GetCamera(cameraId);
    AssertRelease(c);

    bgfx::setViewClear(RenderPassId::FEEDBACK_PASS, BGFX_CLEAR_COLOR, 0);

    bgfx::setViewFrameBuffer(Rendering::RenderPassId::FEEDBACK_PASS, FFramebuffer->GetHandle());

    const glm::uvec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    bgfx::setViewRect(Rendering::RenderPassId::FEEDBACK_PASS, 0, 0, windowSize.x, windowSize.y);

    FDrawCommandBuffer->SetViewTranform(c->GetWorldViewMatrix(), c->GetProjectionMatrix(GLFWDisplayWindowHandler::Instance().AspectRatio()));

    Rendering::CircleFeedbackParameters params = { GameplayConstants::Colony::ColonyInitialRange, GameplayConstants::Colony::ColonyRangeFeedbackThickness,
        GameplayConstants::Colony::ColonyRangeFeedbackColor };
    FDrawCommandBuffer->DrawCircle(Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\feedbackmaterial.material"), params);
    params.Range = params.Range / 2.f;
    FDrawCommandBuffer->DrawCircle(Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\feedbackmaterial.material"), params);

    FDrawCommandBuffer->Submit();
}

} // namespace Rendering
} // namespace ECSEngine