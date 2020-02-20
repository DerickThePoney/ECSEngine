#include "stdafx.h"

#include "ImguiRenderer.h"

#include "BGFXRenderingUtils.h"
#include "DisplayWindow.h"
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

    void Init();
    void Render(ImDrawData* parData);

    void Shutdown();

private:
    void InitMouseAndButtons(ImGuiIO& io);

private:
    ImGuiContext* FImguiContext;
    bgfx::VertexLayout FVertexLayout;
    bgfx::ProgramHandle FProgam;
    bgfx::ProgramHandle FImageProgram;
    bgfx::TextureHandle FTextureHandle;
    bgfx::UniformHandle FTextureSampleUniform;
    bgfx::UniformHandle FImageLodEnabledUniform;
    // ImFont* m_font[ImGui::Font::Count];
    int64_t m_last;
    int32_t m_lastScroll;
    bgfx::ViewId m_viewId;
};

void ImguiRenderer::Init()
{
    m_viewId = 255;
    m_lastScroll = 0;
    m_last = bx::getHPCounter();

    FImguiContext = ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();

    io.DisplaySize = ImVec2(1280.0f, 720.0f);
    io.DeltaTime = 1.0f / 60.0f;
    io.IniFilename = NULL;

    // setupStyle(true);

    bgfx::RendererType::Enum type = bgfx::getRendererType();
    FProgam = LoadProgram("D:\\Programmation\\GameEngine\\ECSEngine\\Assets\\shaders\\Perso\\", "ImGUI", "ocornut_imgui");

    FImageLodEnabledUniform = bgfx::createUniform("u_imageLodEnabled", bgfx::UniformType::Vec4);
    FImageProgram = LoadProgram("D:\\Programmation\\GameEngine\\ECSEngine\\Assets\\shaders\\Perso\\", "ImGUI", "imgui_image");

    FVertexLayout.begin()
          .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
          .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
          .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
          .end();

    FTextureSampleUniform = bgfx::createUniform("s_tex", bgfx::UniformType::Sampler);

    uint8_t* data;
    int32_t width;
    int32_t height;

    io.Fonts->GetTexDataAsRGBA32(&data, &width, &height);

    FTextureHandle = bgfx::createTexture2D((uint16_t)width, (uint16_t)height, false, 1, bgfx::TextureFormat::BGRA8, 0, bgfx::copy(data, width * height * 4));

    DisplayWindow::Instance().InitInputsForImGui(io);
}

void ImguiRenderer::Render(ImDrawData* parDrawData)
{
    const ImGuiIO& io = ImGui::GetIO();
    const float width = io.DisplaySize.x;
    const float height = io.DisplaySize.y;

    bgfx::setViewName(m_viewId, "ImGui");
    bgfx::setViewMode(m_viewId, bgfx::ViewMode::Sequential);

    const bgfx::Caps* caps = bgfx::getCaps();
    {
        float ortho[16];
        bx::mtxOrtho(ortho, 0.0f, width, height, 0.0f, 0.0f, 1000.0f, 0.0f, caps->homogeneousDepth);
        bgfx::setViewTransform(m_viewId, NULL, ortho);
        bgfx::setViewRect(m_viewId, 0, 0, uint16_t(width), uint16_t(height));
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
                    union {
                        ImTextureID ptr;
                        struct
                        {
                            bgfx::TextureHandle handle;
                            uint8_t flags;
                            uint8_t mip;
                        } s;
                    } texture = { cmd->TextureId };
                    state |= 0 != (IMGUI_FLAGS_ALPHA_BLEND & texture.s.flags) ? BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA) : BGFX_STATE_NONE;
                    th = texture.s.handle;
                    if (0 != texture.s.mip)
                    {
                        const float lodEnabled[4] = { float(texture.s.mip), 1.0f, 0.0f, 0.0f };
                        bgfx::setUniform(FImageLodEnabledUniform, lodEnabled);
                        program = FImageProgram;
                    }
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
                bgfx::submit(m_viewId, program);
            }

            offset += cmd->ElemCount;
        }
    }
}

void ImguiRenderer::Shutdown()
{
    ImGui::DestroyContext(FImguiContext);

    bgfx::destroy(FTextureSampleUniform);
    bgfx::destroy(FTextureHandle);

    bgfx::destroy(FImageLodEnabledUniform);
    bgfx::destroy(FImageProgram);
    bgfx::destroy(FProgam);
}

void ImguiRenderer::InitMouseAndButtons(ImGuiIO& io)
{
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos; // We can honor io.WantSetMousePos requests (optional, rarely used)
    io.BackendPlatformName = "imgui_impl_glfw";

    DisplayWindow::Instance().InitInputsForImGui(io);
}

namespace ImGUI
{
void Init()
{
    ECSEngine::Rendering::ImguiRenderer::CreateIFP();
    ECSEngine::Rendering::ImguiRenderer::Instance().Init();
}

void NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = 1.0f / 60.0f; // set the time elapsed since the previous frame (in seconds)
    glm::uvec2 winSize = DisplayWindow::Instance().GetSize();
    io.DisplaySize.x = (float)winSize.x; // set the current display width
    io.DisplaySize.y = (float)winSize.y; // set the current display height here
    /*TODOIMGUI
    glfwGetFramebufferSize(g_Window, &display_w, &display_h);
    if (w > 0 && h > 0)
        io.DisplayFramebufferScale = ImVec2((float)display_w / w, (float)display_h / h);*/

    DisplayWindow::Instance().UpdateMousePosAndButtonsForImGUI(io);
    // ImGui_ImplGlfw_UpdateMouseCursor();

    //// Update game controllers (if enabled and available)
    // ImGui_ImplGlfw_UpdateGamepads();

    ImGui::NewFrame();
}

void Render()
{
    ImGui::EndFrame();
    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    ECSEngine::Rendering::ImguiRenderer::Instance().Render(draw_data);
}

void Shutdown()
{
    ECSEngine::Rendering::ImguiRenderer::Instance().Shutdown();
    ECSEngine::Rendering::ImguiRenderer::Destroy();
}
} // namespace ImGUI

} // namespace Rendering
} // namespace ECSEngine