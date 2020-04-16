#include "stdafx.h"

#include "BGFXRenderer.h"

#include "DrawCommands.h"
#include "GLFWDisplayWindowHandler.h"

#include <bgfx/bgfx.h>

namespace ECSEngine
{
namespace Rendering
{
BGFXRenderer::BGFXRenderer()
{
}

BGFXRenderer::~BGFXRenderer()
{
}

void BGFXRenderer::Init()
{
    AssertRelease(GLFWDisplayWindowHandler::HasInstance());
    GLFWDisplayWindowHandler& window = GLFWDisplayWindowHandler::Instance();
    bgfx::PlatformData pd;
    pd.nwh = window.GetNativeWindowHandle();

    bgfx::Init bgfxInit;
    bgfxInit.platformData = pd;
    bgfxInit.type = bgfx::RendererType::Count; // Automatically choose a renderer.
    bgfxInit.resolution.width = window.GetSize().x;
    bgfxInit.resolution.height = window.GetSize().y;
    bgfxInit.resolution.reset = BGFX_RESET_VSYNC;
    bgfx::init(bgfxInit);

#ifdef ENABLE_BGFX_PROFILING
    bgfx::setDebug(BGFX_DEBUG_PROFILER);
#endif

    bgfx::RendererType::Enum chosenType = bgfx::getRendererType();

    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, window.GetSize().x, window.GetSize().y);
}

void BGFXRenderer::Shutdown()
{
    bgfx::shutdown();
}
void BGFXRenderer::RenderFrame()
{
    std::vector<DrawCommandBuffer*>& commandBuffers = FCommandBuffers[0];

    foreachitem(commandBuffer, commandBuffers) { commandBuffer->Submit(); }

    // Set view 0 default viewport.
    bgfx::touch(0);

    bgfx::frame();

    foreachitem(commandBuffer, commandBuffers) { delete commandBuffer; }
    commandBuffers.clear();
}

void BGFXRenderer::Resize(u32 width, u32 height)
{
    bgfx::reset(width, height, BGFX_RESET_VSYNC);
    bgfx::setViewRect(0, 0, 0, width, height);
}

bool BGFXRenderer::IsInstancingEnabled()
{
    // Get renderer capabilities info.
    const bgfx::Caps* caps = bgfx::getCaps();
    return !(0 == (BGFX_CAPS_INSTANCING & caps->supported));
}

DrawCommandBuffer& BGFXRenderer::CreateCommandBuffer(u16 parViewId /*= 0*/)
{
    DrawCommandBuffer* buffer = new DrawCommandBuffer();

    FCommandBuffers[parViewId].push_back(buffer);

    return *buffer;
}

} // namespace Rendering
} // namespace ECSEngine