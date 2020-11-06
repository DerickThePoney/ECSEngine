#include "stdafx.h"

#include "BGFXRenderer.h"

#include "Common/Logger.h"
#include "DrawCommands.h"
#include "GLFWDisplayWindowHandler.h"

#include <bgfx/bgfx.h>

namespace ECSEngine
{
namespace Rendering
{
BGFXRenderer::BGFXRenderer()
    : FCurrentFrame(0)
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
    bgfxInit.type = bgfx::RendererType::Direct3D11; // Automatically choose a renderer.
    bgfxInit.resolution.width = window.GetSize().x;
    bgfxInit.resolution.height = window.GetSize().y;
    bgfxInit.resolution.reset = BGFX_RESET_VSYNC;
    bgfx::init(bgfxInit);

#ifdef ENABLE_BGFX_PROFILING
    bgfx::setDebug(BGFX_DEBUG_PROFILER);
#endif

    bgfx::RendererType::Enum chosenType = bgfx::getRendererType();

    bgfx::setViewClear(RenderPassId::GEOMETRY_PASS, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x00000000, 1.0f, 0);
    bgfx::setViewRect(RenderPassId::GEOMETRY_PASS, 0, 0, window.GetSize().x, window.GetSize().y);
    bgfx::setViewRect(RenderPassId::DEBUG_PASS, 0, 0, window.GetSize().x, window.GetSize().y);
}

void BGFXRenderer::Shutdown()
{
    bgfx::shutdown();
}
void BGFXRenderer::RenderFrame()
{
    // Set view 0 default viewport.
    bgfx::touch(0);

    FCurrentFrame = bgfx::frame();

    auto itObserversForThisFrame = FSpecificFrameObserver.find(FCurrentFrame);
    if (itObserversForThisFrame != FSpecificFrameObserver.end())
    {
        foreachitem(observer, itObserversForThisFrame->second) { observer(); }
        FSpecificFrameObserver.erase(FCurrentFrame);
    }
}

void BGFXRenderer::Resize(u32 width, u32 height)
{
    bgfx::reset(width, height, BGFX_RESET_VSYNC);
    bgfx::setViewRect(RenderPassId::GEOMETRY_PASS, 0, 0, width, height);
    bgfx::setViewRect(RenderPassId::DEBUG_PASS, 0, 0, width, height);
}

bool BGFXRenderer::IsInstancingEnabled()
{
    // Get renderer capabilities info.
    const bgfx::Caps* caps = bgfx::getCaps();
    return !(0 == (BGFX_CAPS_INSTANCING & caps->supported));
}

DrawCommandBuffer* BGFXRenderer::CreateCommandBuffer(RenderPassId::Type parViewId /*= RenderPassId::GEOMETRY_PASS */)
{
    DrawCommandBuffer* buffer = new DrawCommandBuffer(parViewId);

    return buffer;
}

void BGFXRenderer::ReleaseCommandBuffer(DrawCommandBuffer* buffer)
{
    delete buffer;
}

void BGFXRenderer::AddRequestOnSpecificFrame(const u32 parFrameNumber, const SpecificFrameObserver& parObserver)
{
    FSpecificFrameObserver[parFrameNumber].push_back(parObserver);
}

void BGFXRenderer::DrawStats(bool* outOpen)
{
    const bgfx::Stats* stats = bgfx::getStats();
    AssertRelease(stats != nullptr);

    ImGui::Begin("BGFX Stats", outOpen, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Timings");
    ImGui::Text("CPU frame: %f", (double)stats->cpuTimeFrame / stats->cpuTimerFreq);
    ImGui::Text("GPU frame: %f", (double)(stats->gpuTimeEnd - stats->gpuTimeBegin) / stats->gpuTimerFreq);

    ImGui::Text("Wait for render: %f", (double)stats->waitRender / stats->cpuTimerFreq);
    ImGui::Text("Wait for submit: %f", (double)stats->waitSubmit / stats->cpuTimerFreq);

    ImGui::Separator();
    ImGui::Text("Draw numbers");
    ImGui::Text("Draw number: %d", stats->numDraw);
    ImGui::Text("Compute number: %d", stats->numCompute);
    ImGui::Text("Blit number: %d", stats->numBlit);
    ImGui::Text("Max GPU Driver latency: %d", stats->maxGpuLatency);

    ImGui::Separator();

    ImGui::Text("Buffers statistics");
    ImGui::Text("Num dynamic index buffers: %d", stats->numDynamicIndexBuffers);
    ImGui::Text("Num dynamic vertex buffers: %d", stats->numDynamicVertexBuffers);
    ImGui::Text("Num frame buffers: %d", stats->numFrameBuffers);
    ImGui::Text("Num index buffers: %d", stats->numIndexBuffers);
    ImGui::Text("Num occlusion queries: %d", stats->numOcclusionQueries);
    ImGui::Text("Num programs: %d", stats->numPrograms);
    ImGui::Text("Num shaders: %d", stats->numShaders);
    ImGui::Text("Num textures: %d", stats->numTextures);
    ImGui::Text("Num uniforms: %d", stats->numUniforms);
    ImGui::Text("Num vertex buffers: %d", stats->numVertexBuffers);
    ImGui::Text("Num vertex layouts: %d", stats->numVertexLayouts);

    ImGui::Separator();
    ImGui::Text("Memory statistics");

    ImGui::Text("GPU memory Max: %d", stats->gpuMemoryMax);
    ImGui::Text("GPU memory used: %d", stats->gpuMemoryUsed);
    ImGui::Text("Texture memory used: %d", stats->textureMemoryUsed);
    ImGui::Text("transient VB used: %d", stats->transientVbUsed);
    ImGui::Text("transient IB used: %d", stats->transientIbUsed);

    ImGui::End();
}

} // namespace Rendering
} // namespace ECSEngine