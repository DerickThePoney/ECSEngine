#include "stdafx.h"

#include "UIRenderer.h"

#include "Common/ColorUtils.h"
#include "Common/FixedSizedArray.h"
#include "Common/RandomGenerator.h"
#include "Common/RenderingHandles.h"
#include "Common/Resource.h"
#include "Common/Singleton.h"
#include "Common/TimeManager.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/IndexBuffer.h"
#include "RenderingCore/Material.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/RenderPass.h"
#include "RenderingCore/RenderingState.h"
#include "RenderingCore/UICommandBuffer.h"
#include "RenderingCore/VertexBuffer.h"
#include "RenderingCore/VertexLayout.h"
#include "UICore/Font.h"
#include "UICore/PanelWidgets.h"
#include "UICore/Widget.h"
#include "UICore/WidgetScaler.h"

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
    Rendering::MaterialInstanceHandle FUIVertexColorMaterial;

    std::vector<std::unique_ptr<UI::PanelWidgetDescriptor>> FPanels;
    std::vector<UI::WidgetPtr> FUIElementsForTest;

    UICommandBuffer FBuffer;
    UI::Font FFont;

    bool FPing = true;
    float FAddElement = AddElement;

    DynamicVertexBuffer vertices;
    DynamicIndexBuffer indicesBuffer;
};

UIRenderer::UIRenderer()
{
}

void UIRenderer::Initialise()
{
    FUIVertexColorMaterial = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\uisimpletextmaterial.material");
    using namespace UI;

    std::unique_ptr<PanelWidgetDescriptor> panelDesc = std::make_unique<PanelWidgetDescriptor>();
    WidgetPlacement& placement = panelDesc->InitialPlacement();
    placement.FSizeType = WidgetSizeType::ABSOLUTE_PIXEL;
    placement.FSize = glm::vec2(200.0f, 30.f);
    placement.FPositionFromAnchor = glm::vec2(200.f);

    panelDesc->SetColor(ColorUtils::ConvertToU32(glm::vec4(1.f, 0.f, 0.f, 0.5f)));

    FPanels.push_back(std::move(panelDesc));

    Resource r("Fonts\\OpenSans-Regular.ttf");
    FFont.InitFromResource(r);
}

void UIRenderer::Shutdown()
{
}

void UIRenderer::RenderScene()
{
    if (!FPanels.empty() && FUIElementsForTest.empty())
    {
        foreachitem(panel, FPanels) { FUIElementsForTest.push_back(panel->CreateWidgetAndHierarchy()); }
    }

    UI::WidgetScaler scaler;
    foreachitem(uiel, FUIElementsForTest) { uiel->UpdatePlacement(&scaler); }

    bgfx::setViewName(RenderPassId::GAME_UI_PASS, "GAME_UI_PASS");
    bgfx::setViewMode(RenderPassId::GAME_UI_PASS, bgfx::ViewMode::Sequential);

    auto size = GLFWDisplayWindowHandler::Instance().GetSize();
    glm::mat4 transform = glm::ortho(0.f, (float)size.x, (float)size.y, 0.f);
    bgfx::setViewTransform(RenderPassId::GAME_UI_PASS, NULL, &transform);
    bgfx::setViewRect(RenderPassId::GAME_UI_PASS, 0, 0, uint16_t(size.x), uint16_t(size.y));

    FBuffer.Clear();

    const glm::vec2 scale = scaler.GetScale();
    FBuffer.PushContext(glm::vec4(0.f, 0.f, scale.x, scale.y));

    foreachitemconst(element, FUIElementsForTest) { element->Draw(FBuffer); }

    FBuffer.PopContext();

    FBuffer.Submit();

    bgfx::setViewScissor(RenderPassId::GAME_UI_PASS, 0, 0, size.x, size.y);

    std::string testString = "Test string to rasterize";
    Rendering::VertexLayoutHash vertexHash(true, 1, 1, false, false, false, false);
    Rendering::VertexDataStream stream(testString.size() * 4, vertexHash.GetByteSize(), vertexHash, true);
    std::vector<u32> indices;

    FFont.RasterizeText(testString, glm::vec2(200.f), stream, indices);
    if (!vertices.HandleHasBeenComputed())
        vertices.SetHash(vertexHash);
    vertices.PushRawBuffer(stream);

    indicesBuffer.PushData(indices.data(), (u32)indices.size());

    const MaterialInstance* materialInstance = MaterialManager::GetMaterialInstance(FUIVertexColorMaterial);
    auto& slot = materialInstance->GetMaterialDescriptor()->GetTexturesInput()[0];
    MaterialManager::SetSamplerUniform(slot.GetTextureSlotName(), FFont.FontTexture(), slot.GetSlot());

    RenderingState state(0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));
    state.ApplyState();

    bgfx::setVertexBuffer(0, vertices.GetVertexBufferHandle(), 0u, vertices.GetNumberOfVertices(), vertices.GetVertexLayoutHandle());
    bgfx::setIndexBuffer(indicesBuffer.GetIndexBufferHandle(), 0u, indicesBuffer.GetSize());
    bgfx::submit(RenderPassId::GAME_UI_PASS, materialInstance->GetProgram()->ProgramHandle());
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
