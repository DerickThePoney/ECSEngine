#include "stdafx.h"

#include "FinalCombinePass.h"

#include "RenderingCore/BGFXRenderingBackend.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/MaterialManager.h"

namespace ECSEngine
{
namespace Rendering
{

void FinalCombinePass::Initialise()
{
    FCommandBuffer = BGFXRenderingBackend::Instance().CreateCommandBuffer(RenderPassId::FINAL_COMBINE_PASS);
    bgfx::setViewClear(RenderPassId::FINAL_COMBINE_PASS, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x00000000, 1.0f, 0);
    bgfx::setViewName(RenderPassId::FINAL_COMBINE_PASS, "Final combine pass");
}

void FinalCombinePass::Shutdown()
{
    Rendering::BGFXRenderingBackend::Instance().ReleaseCommandBuffer(FCommandBuffer);
}

void FinalCombinePass::SetTextures(u16 parGameTexture, u16 parUiTexture)
{
    MaterialManager::SetSamplerUniform_IKNOWWHATIMDOING("s_GeometryTexture", parGameTexture, 0);
    MaterialManager::SetSamplerUniform_IKNOWWHATIMDOING("s_FeedbackTexture", parUiTexture, 1);
}

void FinalCombinePass::Render()
{
    SCOPED_PROFILE_SIMPLE;
    auto size = GLFWDisplayWindowHandler::Instance().GetSize();
    bgfx::setViewRect(RenderPassId::FINAL_COMBINE_PASS, 0, 0, size.x, size.y);

    FCommandBuffer->clear();
    MaterialInstanceHandle combineMaterial = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\combinepass.material");

    FCommandBuffer->BlitWithMaterial(combineMaterial);
    FCommandBuffer->Submit();
}

} // namespace Rendering
} // namespace ECSEngine