#include "stdafx.h"

#include "GameRenderer.h"

#include "RenderingCore/BGFXRenderingBackend.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/Framebuffer.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/RenderPass.h"

namespace ECSEngine
{
namespace Rendering
{

void GameRenderer::Initialise()
{
    auto& size = GLFWDisplayWindowHandler::Instance().GetSize();

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
    FFeedbackFramebuffer = new FramebufferInstance(FramebufferSizeType::SCREEN, size);

    FFeedbackFramebuffer->AddAttachement(false, 1, bgfx::TextureFormat::RGBA8,
          0 | BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);

    FFeedbackFramebuffer->InitFramebuffer();

    FFeedbackCommandBuffer = Rendering::BGFXRenderingBackend::Instance().CreateCommandBuffer(RenderPassId::FEEDBACK_PASS);
    bgfx::setViewClear(RenderPassId::FEEDBACK_PASS, BGFX_CLEAR_COLOR, 0x00000000);

    // Combine pass
    bgfx::setViewClear(RenderPassId::COMBINE_PASS, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x00000000, 1.0f, 0);

    bgfx::setViewRect(RenderPassId::GEOMETRY_PASS, 0, 0, size.x, size.y);
    bgfx::setViewRect(RenderPassId::FEEDBACK_PASS, 0, 0, size.x, size.y);
    bgfx::setViewRect(RenderPassId::COMBINE_PASS, 0, 0, size.x, size.y);
    bgfx::setViewRect(RenderPassId::DEBUG_PASS, 0, 0, size.x, size.y);
}

void GameRenderer::Shutdown()
{
    Rendering::BGFXRenderingBackend::Instance().ReleaseCommandBuffer(FGeometryCommandBuffer);
    Rendering::BGFXRenderingBackend::Instance().ReleaseCommandBuffer(FFeedbackCommandBuffer);

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

    // Resize framebuffers
    auto& size = GLFWDisplayWindowHandler::Instance().GetSize();
    FGeometryFramebuffer->ResizeIFN(size);
    FFeedbackFramebuffer->ResizeIFN(size);

    // Geometry pass

    // feedback pass

    // combine pass
}

} // namespace Rendering
} // namespace ECSEngine