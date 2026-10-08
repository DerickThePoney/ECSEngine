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
#include "bx/bx.h"
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

// Packed into ImTextureID for backend-managed textures (font atlas). Must be 8 bytes.
struct ImGuiBgfxTexture
{
    bgfx::TextureHandle handle;
    uint16_t flags;
    uint32_t unused;
};
static_assert(sizeof(ImGuiBgfxTexture) == sizeof(ImTextureID), "ImGuiBgfxTexture must match ImTextureID size");

ImTextureID PackImGuiTexture(bgfx::TextureHandle handle)
{
    ImGuiBgfxTexture tex{ handle, IMGUI_FLAGS_ALPHA_BLEND, 0 };
    return bx::bitCast<ImTextureID>(tex);
}

bgfx::TextureHandle UnpackImGuiTexture(ImTextureID id)
{
    return bx::bitCast<ImGuiBgfxTexture>(id).handle;
}

void UpdateImGuiTexture(ImTextureData* texData)
{
    switch (texData->Status)
    {
    case ImTextureStatus_WantCreate:
    {
        AssertRelease(texData->Format == ImTextureFormat_RGBA32 || texData->BytesPerPixel == 4);

        const bgfx::TextureHandle handle = bgfx::createTexture2D((uint16_t)texData->Width, (uint16_t)texData->Height, false, 1, bgfx::TextureFormat::BGRA8, 0);
        bgfx::setName(handle, "ImGui Font Atlas");
        bgfx::updateTexture2D(handle, 0, 0, 0, 0, (uint16_t)texData->Width, (uint16_t)texData->Height, bgfx::copy(texData->GetPixels(), texData->GetSizeInBytes()));

        texData->SetTexID(PackImGuiTexture(handle));
        texData->SetStatus(ImTextureStatus_OK);
        break;
    }

    case ImTextureStatus_WantUpdates:
    {
        const bgfx::TextureHandle handle = UnpackImGuiTexture(texData->GetTexID());
        AssertRelease(bgfx::isValid(handle));

        for (ImTextureRect& rect : texData->Updates)
        {
            const uint32_t bpp = (uint32_t)texData->BytesPerPixel;
            const bgfx::Memory* pix = bgfx::alloc(rect.h * rect.w * bpp);
            bx::gather(pix->data, (const uint8_t*)texData->GetPixelsAt(rect.x, rect.y), texData->GetPitch(), rect.w * bpp, rect.h);
            bgfx::updateTexture2D(handle, 0, 0, (uint16_t)rect.x, (uint16_t)rect.y, (uint16_t)rect.w, (uint16_t)rect.h, pix);
        }

        texData->SetStatus(ImTextureStatus_OK);
        break;
    }

    case ImTextureStatus_WantDestroy:
    {
        if (texData->UnusedFrames > 0)
        {
            const ImTextureID id = texData->GetTexID();
            if (id != ImTextureID_Invalid)
            {
                const bgfx::TextureHandle handle = UnpackImGuiTexture(id);
                if (bgfx::isValid(handle))
                    bgfx::destroy(handle);
            }
            texData->SetTexID(ImTextureID_Invalid);
            texData->SetStatus(ImTextureStatus_Destroyed);
        }
        break;
    }

    default:
        break;
    }
}

void DestroyAllImGuiTextures()
{
    for (ImTextureData* texData : ImGui::GetPlatformIO().Textures)
    {
        if (texData->Status == ImTextureStatus_Destroyed)
            continue;

        const ImTextureID id = texData->GetTexID();
        if (id != ImTextureID_Invalid)
        {
            const bgfx::TextureHandle handle = UnpackImGuiTexture(id);
            if (bgfx::isValid(handle))
                bgfx::destroy(handle);
        }
        texData->SetTexID(ImTextureID_Invalid);
        texData->SetStatus(ImTextureStatus_Destroyed);
    }
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
    bgfx::UniformHandle FTextureSampleUniform;
    std::shared_ptr<ECSEngine::ResourceHandle> FFontDataHandle;
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

    io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;
    io.BackendRendererName = "ECSEngine_bgfx";

    ImGui::StyleColorsDark();

    io.DisplaySize = ImVec2(1280.0f, 720.0f);
    io.DeltaTime = 1.0f / 60.0f;
    io.IniFilename = NULL;

    {
        ECSEngine::Resource styleResource(std::format(
              "Styles\\Style_{}.style", Rendering::RenderPassId::GetNameFromType((Rendering::RenderPassId::Type)(parContext + Rendering::RenderPassId::IMGUI_PASSES_START))));
        if (GlobalResourceCache::Instance().FCache->FileExists(&styleResource))
        {
            std::shared_ptr<ECSEngine::ResourceHandle> styleResHandle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&styleResource);
            AssertRelease(styleResHandle != nullptr);
            // ImGuiStyle layout changed in 1.92; only apply blobs that match the current size.
            if (styleResHandle->Size() == sizeof(ImGuiStyle))
            {
                memcpy(&ImGui::GetStyle(), styleResHandle->WritableBuffer(), sizeof(ImGuiStyle));
            }
        }
    }

    ImGuiStyle& style = ImGui::GetStyle();
    if (style.FontScaleDpi <= 0.f)
        style.FontScaleDpi = 1.f;
    if (style.FontScaleMain <= 0.f)
        style.FontScaleMain = 1.f;
    if (style.FontSizeBase <= 0.f)
        style.FontSizeBase = 20.f;

    GLFWDisplayWindowHandler::Instance().InitInputsForImGui(io);

    if (parContext == 0)
    {
        FProgam = LoadProgram("Shaders\\Perso\\", "ImGUI", "ocornut_imgui");

        FVertexLayout.begin()
              .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
              .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
              .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
              .end();

        FTextureSampleUniform = bgfx::createUniform("s_tex", bgfx::UniformType::Sampler);

        ECSEngine::ResourceCache* cache = ECSEngine::GlobalResourceCache::Instance().FCache;
        ECSEngine::Resource fontResource("Fonts\\OpenSans-Regular.ttf");
        // Keep TTF bytes alive for the atlas lifetime (required since ImGui 1.92 / FontDataOwnedByAtlas = false).
        FFontDataHandle = cache->GetResourceHandle(&fontResource);
        AssertRelease(FFontDataHandle != nullptr);

        ImFontConfig config;
        config.FontDataOwnedByAtlas = false;
        config.MergeMode = false;
        io.Fonts->AddFontFromMemoryTTF(FFontDataHandle->WritableBuffer(), (int)FFontDataHandle->Size(), 20.f, &config);
        // Texture upload is deferred to Render() via ImDrawData::Textures (RendererHasTextures).
    }
}

void ImguiRenderer::Render(ImDrawData* parDrawData, const u16 parViewId)
{
    if (parDrawData->Textures != nullptr)
    {
        for (ImTextureData* texData : *parDrawData->Textures)
        {
            if (texData->Status != ImTextureStatus_OK)
                UpdateImGuiTexture(texData);
        }
    }

    const float width = parDrawData->DisplaySize.x;
    const float height = parDrawData->DisplaySize.y;
    if (width <= 0.f || height <= 0.f || parDrawData->CmdLists.Size == 0)
        return;

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
    for (int32_t ii = 0, num = parDrawData->CmdLists.Size; ii < num; ++ii)
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
                uint64_t state = 0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA
                                 | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);

                bgfx::TextureHandle th = BGFX_INVALID_HANDLE;
                bgfx::ProgramHandle program = FProgam;

                if (cmd->TexRef._TexData != nullptr)
                {
                    // Backend-managed texture (font atlas): ImTextureID packs a bgfx handle.
                    th = UnpackImGuiTexture(cmd->GetTexID());
                }
                else if (cmd->GetTexID() != ImTextureID_Invalid)
                {
                    // User texture: ImTextureID is a pointer to Rendering::TextureHandle (e.g. ImageButton).
                    const Rendering::TextureHandle* textureHandle = (const Rendering::TextureHandle*)(uintptr_t)cmd->GetTexID();
                    const Rendering::Texture* textureToDisplay = Rendering::TextureManager::Instance().GetTexture(*textureHandle);
                    AssertRelease(textureToDisplay != nullptr);
                    AssertRelease(textureToDisplay->Valid());
                    th = textureToDisplay->Handle();
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

    if (parContext == 0)
    {
        SetCurrentContext(0);
        DestroyAllImGuiTextures();
    }

    ImGui::DestroyContext(FImguiContexts[parContext]);
    FImguiContexts[parContext] = nullptr;

    if (parContext == 0)
    {
        bgfx::destroy(FTextureSampleUniform);
        bgfx::destroy(FProgam);
        FFontDataHandle.reset();
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
        // Always call Render so ImDrawData::Textures (WantCreate/Update/Destroy) are processed.
        ECSEngine::Rendering::ImguiRenderer::Instance().Render(draw_data, i);
    }
}

void Shutdown()
{
    // Destroy non-owning contexts first; context 0 owns the shared font atlas / backend textures.
    for (u16 i = RenderPassId::IMGUI_PASSES_END + 1; i-- > RenderPassId::IMGUI_PASSES_START;)
    {
        ECSEngine::Rendering::ImguiRenderer::Instance().Shutdown(i - RenderPassId::IMGUI_PASSES_START);
    }
    ECSEngine::Rendering::ImguiRenderer::Destroy();
}
} // namespace ImGUI

} // namespace Rendering
} // namespace ECSEngine
