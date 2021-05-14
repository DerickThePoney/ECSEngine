#include "stdafx.h"

#include "ImguiRenderer.h"

#include "BGFXRenderingUtils.h"
#include "Common/FixedSizedArray.h"
#include "Common/InputManager.h"
#include "Common/RenderingHandles.h"
#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"
#include "Common/TimeManager.h"
#include "GLFWDisplayWindowHandler.h"
#include "Texture.h"
#include "TexturesManager.h"
#include "bx/math.h"

#include <bx/timer.h>

namespace ECSEngine
{
namespace Rendering
{

namespace
{
inline bool checkAvailTransientBuffers(uint32_t _numVertices, const bgfx::VertexLayout& _layout, uint32_t _numIndices)
{
    return _numVertices == bgfx::getAvailTransientVertexBuffer(_numVertices, _layout) && (0 == _numIndices || _numIndices == bgfx::getAvailTransientIndexBuffer(_numIndices));
}
} // namespace

class ImguiRenderer : public Singleton<ImguiRenderer>
{
public:
    ImguiRenderer()
        : Singleton()
    {
    }

    void Init(const u32 parContext);
    void Render(ImDrawData* parData, const u16 parViewId);

    void SetCurrentContext(const u32 parContext);

    void Shutdown(const u32 parContext);

    const u32 GetContextNumber() const { return FImguiContexts.size(); }

private:
    void InitMouseAndButtons(ImGuiIO& io);

private:
    static constexpr u32 PassesNumber = RenderPassId::IMGUI_PASSES_END - RenderPassId::IMGUI_PASSES_START + 1;
    FixedSizedArrayInSitu<ImGuiContext*, PassesNumber> FImguiContexts;
    bgfx::VertexLayout FVertexLayout;
    bgfx::ProgramHandle FProgam;
    bgfx::TextureHandle FTextureHandle;
    bgfx::UniformHandle FTextureSampleUniform;
};

void ImguiRenderer::Init(const u32 parContext)
{
    ImFontAtlas* sharedFontAtlas = nullptr;
    if (parContext != 0)
    {
        AssertRelease(FImguiContexts[0] != nullptr);
        SetCurrentContext(0);
        ImGuiIO& io = ImGui::GetIO();
        sharedFontAtlas = io.Fonts;
    }
    AssertRelease(FImguiContexts[parContext] == nullptr);
    FImguiContexts[parContext] = ImGui::CreateContext(sharedFontAtlas);
    SetCurrentContext(parContext);

    ImGuiIO& io = ImGui::GetIO();

    io.DisplaySize = ImVec2(1280.0f, 720.0f);
    io.DeltaTime = 1.0f / 60.0f;
    io.IniFilename = NULL;

    {
        ECSEngine::Resource styleResource(
              fmt::format("Styles\\Style_{}.style", Rendering::RenderPassId::GetName((Rendering::RenderPassId::Type)(parContext + Rendering::RenderPassId::IMGUI_PASSES_START))));
        if (GlobalResourceCache::Instance().FCache->FileExists(&styleResource))
        {
            std::shared_ptr<ECSEngine::ResourceHandle> styleResHandle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&styleResource);
            AssertRelease(styleResHandle != nullptr);
            memcpy(&ImGui::GetStyle(), styleResHandle->WritableBuffer(), sizeof(ImGuiStyle));
        }
    }

    GLFWDisplayWindowHandler::Instance().InitInputsForImGui(io);

    if (parContext == 0)
    {
        bgfx::RendererType::Enum type = bgfx::getRendererType();
        FProgam = LoadProgram("Shaders\\Perso\\", "ImGUI", "ocornut_imgui");

        FVertexLayout.begin()
              .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
              .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
              .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
              .end();

        FTextureSampleUniform = bgfx::createUniform("s_tex", bgfx::UniformType::Sampler);

        uint8_t* data;
        int32_t width;
        int32_t height;

        ECSEngine::ResourceCache* cache = ECSEngine::GlobalResourceCache::Instance().FCache;
        ECSEngine::Resource shaderResource("Fonts\\OpenSans-Regular.ttf");
        std::shared_ptr<ECSEngine::ResourceHandle> shaderDataHandle = cache->GetResourceHandle(&shaderResource);
        ImFontConfig config;
        config.FontDataOwnedByAtlas = false;
        config.MergeMode = false;
        io.Fonts->AddFontFromMemoryTTF(shaderDataHandle->WritableBuffer(), shaderDataHandle->Size(), 20, &config);

        io.Fonts->GetTexDataAsRGBA32(&data, &width, &height);

        FTextureHandle = bgfx::createTexture2D((uint16_t)width, (uint16_t)height, false, 1, bgfx::TextureFormat::BGRA8, 0, bgfx::copy(data, width * height * 4));
    }
}

void ImguiRenderer::Render(ImDrawData* parDrawData, const u16 parViewId)
{
    // SHAMELESSLY STOLEN FROM BGFX EXAMPLES...
    const ImGuiIO& io = ImGui::GetIO();
    const float width = io.DisplaySize.x;
    const float height = io.DisplaySize.y;

    bgfx::setViewName(parViewId, "ImGui");
    bgfx::setViewMode(parViewId, bgfx::ViewMode::Sequential);

    const bgfx::Caps* caps = bgfx::getCaps();
    {
        float ortho[16];
        bx::mtxOrtho(ortho, 0.0f, width, height, 0.0f, 0.0f, 1000.0f, 0.0f, caps->homogeneousDepth);
        bgfx::setViewTransform(parViewId, NULL, ortho);
        bgfx::setViewRect(parViewId, 0, 0, uint16_t(width), uint16_t(height));
    }

    // Render command lists
    for (int32_t ii = 0, num = parDrawData->CmdListsCount; ii < num; ++ii)
    {
        bgfx::TransientVertexBuffer tvb;
        bgfx::TransientIndexBuffer tib;

        const ImDrawList* drawList = parDrawData->CmdLists[ii];
        uint32_t numVertices = (uint32_t)drawList->VtxBuffer.size();
        uint32_t numIndices = (uint32_t)drawList->IdxBuffer.size();

        if (!checkAvailTransientBuffers(numVertices, FVertexLayout, numIndices))
        {
            // not enough space in transient buffer just quit drawing the rest...
            break;
        }

        bgfx::allocTransientVertexBuffer(&tvb, numVertices, FVertexLayout);
        bgfx::allocTransientIndexBuffer(&tib, numIndices);

        ImDrawVert* verts = (ImDrawVert*)tvb.data;
        bx::memCopy(verts, drawList->VtxBuffer.begin(), numVertices * sizeof(ImDrawVert));

        ImDrawIdx* indices = (ImDrawIdx*)tib.data;
        bx::memCopy(indices, drawList->IdxBuffer.begin(), numIndices * sizeof(ImDrawIdx));

        uint32_t offset = 0;
        for (const ImDrawCmd *cmd = drawList->CmdBuffer.begin(), *cmdEnd = drawList->CmdBuffer.end(); cmd != cmdEnd; ++cmd)
        {
            if (cmd->UserCallback)
            {
                cmd->UserCallback(drawList, cmd);
            }
            else if (0 != cmd->ElemCount)
            {
                uint64_t state = 0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA;

                bgfx::TextureHandle th = FTextureHandle;
                bgfx::ProgramHandle program = FProgam;

                if (NULL != cmd->TextureId)
                {
                    const Rendering::TextureHandle* textureHandle = (Rendering::TextureHandle*)cmd->TextureId;
                    const Rendering::Texture* textureToDisplay = Rendering::TextureManager::Instance().GetTexture(*textureHandle);
                    AssertRelease(textureToDisplay != nullptr);
                    AssertRelease(textureToDisplay->Valid());

                    state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);
                    th = textureToDisplay->Handle();
                }
                else
                {
                    state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);
                }

                const uint16_t xx = uint16_t(bx::max(cmd->ClipRect.x, 0.0f));
                const uint16_t yy = uint16_t(bx::max(cmd->ClipRect.y, 0.0f));
                bgfx::setScissor(xx, yy, uint16_t(bx::min(cmd->ClipRect.z, 65535.0f) - xx), uint16_t(bx::min(cmd->ClipRect.w, 65535.0f) - yy));

                bgfx::setState(state);
                bgfx::setTexture(0, FTextureSampleUniform, th);
                bgfx::setVertexBuffer(0, &tvb, 0, numVertices);
                bgfx::setIndexBuffer(&tib, offset, cmd->ElemCount);
                bgfx::submit(parViewId, program);
            }

            offset += cmd->ElemCount;
        }
    }
}

void ImguiRenderer::SetCurrentContext(const u32 parContext)
{
    AssertRelease(parContext < PassesNumber);
    AssertRelease(FImguiContexts[parContext] != nullptr);
    ImGui::SetCurrentContext(FImguiContexts[parContext]);
}

void ImguiRenderer::Shutdown(const u32 parContext)
{
    AssertRelease(parContext < PassesNumber);
    AssertRelease(FImguiContexts[parContext] != nullptr);
    ImGui::DestroyContext(FImguiContexts[parContext]);

    if (parContext == 0)
    {
        bgfx::destroy(FTextureSampleUniform);
        bgfx::destroy(FTextureHandle);

        bgfx::destroy(FProgam);
    }
}

void ImguiRenderer::InitMouseAndButtons(ImGuiIO& io)
{
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos; // We can honor io.WantSetMousePos requests (optional, rarely used)
    io.BackendPlatformName = "ECSEngine_GLFW";

    GLFWDisplayWindowHandler::Instance().InitInputsForImGui(io);
}

namespace ImGUI
{
void Init()
{
    ECSEngine::Rendering::ImguiRenderer::CreateIFP();

    for (u16 i = RenderPassId::IMGUI_PASSES_START; i < RenderPassId::IMGUI_PASSES_END + 1; ++i)
    {
        ECSEngine::Rendering::ImguiRenderer::Instance().Init(i - RenderPassId::IMGUI_PASSES_START);
    }
}

void SetImGuiContext(RenderPassId::Type parImGuiPass)
{
    AssertRelease(parImGuiPass >= RenderPassId::IMGUI_PASSES_START && parImGuiPass <= RenderPassId::IMGUI_PASSES_END);
    ImguiRenderer::Instance().SetCurrentContext(parImGuiPass - RenderPassId::IMGUI_PASSES_START);
}

void NewFrame()
{
    bool captureKeyboard = false, captureMouse = false;
    for (u16 i = RenderPassId::IMGUI_PASSES_START; i < RenderPassId::IMGUI_PASSES_END + 1; ++i)
    {
        ImguiRenderer::Instance().SetCurrentContext(i - RenderPassId::IMGUI_PASSES_START);
        ImGuiIO& io = ImGui::GetIO();
        io.DeltaTime = ECSEngine::TimeManager::FrameDeltaTime();
        if (io.DeltaTime == 0.0f)
            io.DeltaTime = 1.f / 60.f;
        io.DisplaySize = GLFWDisplayWindowHandler::Instance().GetSize();

        if (io.DisplaySize.x > 0 && io.DisplaySize.y > 0)
            io.DisplayFramebufferScale = ImVec2((float)1.0f, (float)1.0f);

        GLFWDisplayWindowHandler::Instance().UpdateMousePosAndButtonsForImGUI(io);
        GLFWDisplayWindowHandler::Instance().UpdateMouseCursorForImGUI(io);
        GLFWDisplayWindowHandler::Instance().UpdateJoysticks(io);

        //// Update game controllers (if enabled and available)
        // ImGui_ImplGlfw_UpdateGamepads();
        captureKeyboard = captureKeyboard || io.WantCaptureKeyboard;
        captureMouse = captureMouse || io.WantCaptureMouse;

        ImGui::NewFrame();
    }

    Input::SetInputsAlreadyUsed(captureKeyboard, captureMouse);
}

void Render()
{
    for (u16 i = RenderPassId::IMGUI_PASSES_START; i < RenderPassId::IMGUI_PASSES_END + 1; ++i)
    {
        ImguiRenderer::Instance().SetCurrentContext(i - RenderPassId::IMGUI_PASSES_START);
        ImGui::EndFrame();
        ImGui::Render();
        ImDrawData* draw_data = ImGui::GetDrawData();
        if (draw_data->CmdListsCount > 0)
            ECSEngine::Rendering::ImguiRenderer::Instance().Render(draw_data, i);
    }
}

void Shutdown()
{
    for (u16 i = RenderPassId::IMGUI_PASSES_START; i < RenderPassId::IMGUI_PASSES_END + 1; ++i)
    {
        ECSEngine::Rendering::ImguiRenderer::Instance().Shutdown(i - RenderPassId::IMGUI_PASSES_START);
    }
    ECSEngine::Rendering::ImguiRenderer::Destroy();
}
} // namespace ImGUI

} // namespace Rendering
} // namespace ECSEngine
