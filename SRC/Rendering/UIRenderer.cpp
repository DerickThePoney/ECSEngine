#include "stdafx.h"

#include "UIRenderer.h"

#include "Common/ColorUtils.h"
#include "Common/FixedSizedArray.h"
#include "Common/RandomGenerator.h"
#include "Common/RenderingHandles.h"
#include "Common/Singleton.h"
#include "Common/TimeManager.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/IndexBuffer.h"
#include "RenderingCore/Material.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/RenderPass.h"
#include "RenderingCore/RenderingState.h"
#include "RenderingCore/VertexBuffer.h"
#include "RenderingCore/VertexLayout.h"

namespace ECSEngine
{
namespace Rendering
{

constexpr float AddElement = 2.f;

struct TestUIElement
{
    glm::vec2 Pos;
    glm::vec2 Size;
    u32 Color;
};

class UIRenderer : public Singleton<UIRenderer>
{
public:
    UIRenderer();

    void Initialise();
    void Shutdown();
    void RenderScene();

private:
    void DrawUIElement(const TestUIElement& parUIElement, VertexDataStream& stream, std::vector<u32>& indices);

private:
    Rendering::MaterialInstanceHandle FUIVertexColorMaterial;

    std::vector<TestUIElement> FUIElementsForTest;
    DynamicVertexBuffer FVertexBuffer;
    DynamicIndexBuffer FIndexBuffer;

    bool FPing = true;
    float FAddElement = AddElement;
};

UIRenderer::UIRenderer()
    : FVertexBuffer(VertexLayoutHash(true, 1, 0, false, false, false, false))
{
}

void UIRenderer::Initialise()
{
    FUIVertexColorMaterial = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\uivertexcolormaterial.material");

    FUIElementsForTest.push_back({ glm::vec2(50.f, 50.f), glm::vec2(200.f, 200.f), ColorUtils::ConvertToU32(glm::vec4(0.7f, 0.8f, 0.2f, 1.f)) });

    FUIElementsForTest.push_back({ glm::vec2(70.f, 70.f), glm::vec2(50.f, 50.f), ColorUtils::ConvertToU32(glm::vec4(0.1f, 0.5f, 0.9f, 1.f)) });

    FUIElementsForTest.push_back({ glm::vec2(110.f, 70.f), glm::vec2(50.f, 50.f), ColorUtils::ConvertToU32(glm::vec4(1.f, 0.0f, 0.0f, 1.f)) });

    FUIElementsForTest.push_back({ glm::vec2(70.f, 150.f), glm::vec2(50.f, 50.f), ColorUtils::ConvertToU32(glm::vec4(0.1f, 1.0f, 0.0f, 1.f)) });

    FUIElementsForTest.push_back({ glm::vec2(50.f, 220.f), glm::vec2(50.f, 50.f), ColorUtils::ConvertToU32(glm::vec4(1.0f, 1.0f, 0.0f, 1.f)) });
}

void UIRenderer::Shutdown()
{
}

void UIRenderer::RenderScene()
{
    if (FPing)
    {
        if (FUIElementsForTest.size() < 10)
        {
            if (FAddElement < 0.f)
            {
                const glm::vec2 pos = glm::vec2(20.f + RandomNumbers::NextFloat() * (250.f - 20.f), 20.f + RandomNumbers::NextFloat() * (250.f - 20.f));
                const glm::vec2 size = glm::vec2(20.f + RandomNumbers::NextFloat() * (100.f - 20.f), 20.f + RandomNumbers::NextFloat() * (100.f - 20.f));
                const u32 color = ColorUtils::ConvertToU32(glm::vec4(RandomNumbers::NextFloat(), RandomNumbers::NextFloat(), RandomNumbers::NextFloat(), 1.0f));
                FUIElementsForTest.push_back({ pos, size, color });
                FAddElement = AddElement;
            }
            else
            {
                FAddElement -= TimeManager::FrameDeltaTime();
            }
        }
        else
        {
            FPing = false;
            FAddElement = AddElement;
        }
    }
    else
    {
        if (FUIElementsForTest.size() > 5)
        {
            if (FAddElement < 0.f)
            {
                FUIElementsForTest.pop_back();
                FAddElement = AddElement;
            }
            else
            {
                FAddElement -= TimeManager::FrameDeltaTime();
            }
        }
        else
        {
            FPing = true;
            FAddElement = AddElement;
        }
    }

    bgfx::setViewName(RenderPassId::GAME_UI_PASS, "GAME_UI_PASS");
    bgfx::setViewMode(RenderPassId::GAME_UI_PASS, bgfx::ViewMode::Sequential);

    RenderingState state(0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));

    VertexDataStream stream(4, FVertexBuffer.Hash().GetByteSize(), FVertexBuffer.Hash(), true);
    std::vector<u32> indices;

    glm::vec4 cliprect(FUIElementsForTest[0].Pos.x, FUIElementsForTest[0].Pos.y, FUIElementsForTest[0].Size.x, FUIElementsForTest[0].Size.y);

    foreachitemconst(element, FUIElementsForTest) { DrawUIElement(element, stream, indices); }

    FVertexBuffer.PushRawBuffer(stream);
    FIndexBuffer.PushData(indices.data(), indices.size());

    auto size = GLFWDisplayWindowHandler::Instance().GetSize();
    glm::mat4 transform = glm::ortho(0.f, (float)size.x, (float)size.y, 0.f);

    bgfx::setViewTransform(RenderPassId::GAME_UI_PASS, NULL, &transform);
    bgfx::setViewRect(RenderPassId::GAME_UI_PASS, 0, 0, uint16_t(size.x), uint16_t(size.y));
    state.ApplyState();

    bgfx::setScissor(cliprect.x, cliprect.y, cliprect.z, cliprect.w);

    bgfx::setVertexBuffer(0, FVertexBuffer.GetVertexBufferHandle(), 0u, FVertexBuffer.GetNumberOfVertices(), FVertexBuffer.GetVertexLayoutHandle());
    bgfx::setIndexBuffer(FIndexBuffer.GetIndexBufferHandle(), 0u, FIndexBuffer.GetSize());

    const Rendering::MaterialInstance* instance = Rendering::MaterialManager::GetMaterialInstance(FUIVertexColorMaterial);
    AssertRelease(instance != nullptr);
    bgfx::submit(RenderPassId::GAME_UI_PASS, instance->GetProgram()->ProgramHandle());
}

void UIRenderer::DrawUIElement(const TestUIElement& parUIElement, VertexDataStream& stream, std::vector<u32>& indices)
{
    const u32 indexOffset = (u32)stream.GetSize();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(parUIElement.Pos, 0.f));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, parUIElement.Color);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(parUIElement.Pos + glm::vec2(parUIElement.Size.x, 0.f), 0.f));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, parUIElement.Color);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(parUIElement.Pos + glm::vec2(parUIElement.Size.x, parUIElement.Size.y), 0.f));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, parUIElement.Color);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(parUIElement.Pos + glm::vec2(0.f, parUIElement.Size.y), 0.f));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, parUIElement.Color);
    stream.Advance();

    indices.push_back(indexOffset + 0);
    indices.push_back(indexOffset + 1);
    indices.push_back(indexOffset + 2);

    indices.push_back(indexOffset + 0);
    indices.push_back(indexOffset + 2);
    indices.push_back(indexOffset + 3);
}

namespace UIRendering
{

void Initialise()
{
    UIRenderer::CreateIFP();
    UIRenderer::Instance().Initialise();
}

void Shutdown()
{
    UIRenderer::Instance().Shutdown();
    UIRenderer::Destroy();
}

void RenderScene()
{
    UIRenderer::Instance().RenderScene();
}

} // namespace UIRendering

} // namespace Rendering

} // namespace ECSEngine
