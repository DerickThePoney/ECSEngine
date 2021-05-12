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
#include "RenderingCore/UICommandBuffer.h"
#include "RenderingCore/VertexBuffer.h"
#include "RenderingCore/VertexLayout.h"
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
    void PushUIElement(const UI::WidgetPtr parUIElement);
    void DrawUIElements(const UI::WidgetPtr parUIElement);
    void DrawThisElement(const UI::WidgetPtr parUIElement);

private:
    Rendering::MaterialInstanceHandle FUIVertexColorMaterial;

    std::vector<UI::WidgetPtr> FUIElementsForTest;

    UICommandBuffer FBuffer;

    bool FPing = true;
    float FAddElement = AddElement;
};

UIRenderer::UIRenderer()
{
}

void UIRenderer::Initialise()
{
    FUIVertexColorMaterial = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\uivertexcolormaterial.material");
    using namespace UI;

    {
        TestUIWidget* a = new TestUIWidget();
        WidgetPlacement& aplacement = a->Placement();
        aplacement.FPositioningType = WidgetPositionningType::RELATIVE_POS;
        aplacement.FPositionFromAnchor = glm::vec2(0.5f);
        aplacement.FSelfAnchor = glm::vec2(.5f, .5f);
        aplacement.FSize = glm::vec2(500, 350);
        a->Color = ColorUtils::ConvertToU32(glm::vec4(RandomNumbers::NextFloat(), RandomNumbers::NextFloat(), RandomNumbers::NextFloat(), 1.0f));
        FUIElementsForTest.push_back(WidgetPtr(a));

        TestUIWidget* b = new TestUIWidget();
        WidgetPlacement& bplacement = b->Placement();
        bplacement.FPositioningType = WidgetPositionningType::RELATIVE_POS;
        bplacement.FPositionFromAnchor = glm::vec2(0.5f);
        bplacement.FSelfAnchor = glm::vec2(.5f, .5f);
        bplacement.FSize = glm::vec2(40, 300);
        b->Color = ColorUtils::ConvertToU32(glm::vec4(RandomNumbers::NextFloat(), RandomNumbers::NextFloat(), RandomNumbers::NextFloat(), 1.0f));
        a->AddChild(WidgetPtr(b));
    }

    {
        TestUIWidget* a = new TestUIWidget();
        WidgetPlacement& aplacement = a->Placement();
        aplacement.FPositioningType = WidgetPositionningType::RELATIVE_POS;
        aplacement.FSizeType = WidgetSizeType::ABSOLUTE_RELATIVE;
        aplacement.FPositionFromAnchor = glm::vec2(0.0f);
        aplacement.FSelfAnchor = glm::vec2(0.f);
        aplacement.FSize = glm::vec2(1.f, 0.01f);
        a->Color = ColorUtils::ConvertToU32(glm::vec4(RandomNumbers::NextFloat(), RandomNumbers::NextFloat(), RandomNumbers::NextFloat(), 1.0f));
        FUIElementsForTest.push_back(WidgetPtr(a));

        TestUIWidget* b = new TestUIWidget();
        WidgetPlacement& bplacement = b->Placement();
        bplacement.FPositioningType = WidgetPositionningType::RELATIVE_POS;
        bplacement.FPositionFromAnchor = glm::vec2(0.5f);
        bplacement.FSelfAnchor = glm::vec2(.5f, .5f);
        bplacement.FSize = glm::vec2(40, 300);
        b->Color = ColorUtils::ConvertToU32(glm::vec4(RandomNumbers::NextFloat(), RandomNumbers::NextFloat(), RandomNumbers::NextFloat(), 1.0f));
        a->AddChild(WidgetPtr(b));
    }
}

void UIRenderer::Shutdown()
{
}

void UIRenderer::RenderScene()
{
    UI::WidgetScaler scaler;
    foreachitem(uiel, FUIElementsForTest) { uiel->UpdatePlacement(&scaler); }

    bgfx::setViewName(RenderPassId::GAME_UI_PASS, "GAME_UI_PASS");
    bgfx::setViewMode(RenderPassId::GAME_UI_PASS, bgfx::ViewMode::Sequential);

    auto size = GLFWDisplayWindowHandler::Instance().GetSize();
    glm::mat4 transform = glm::ortho(0.f, (float)size.x, (float)size.y, 0.f);
    bgfx::setViewTransform(RenderPassId::GAME_UI_PASS, NULL, &transform);
    bgfx::setViewRect(RenderPassId::GAME_UI_PASS, 0, 0, uint16_t(size.x), uint16_t(size.y));

    FBuffer.Clear();

    foreachitemconst(element, FUIElementsForTest) { PushUIElement(element); }

    FBuffer.Submit();
}

void UIRenderer::PushUIElement(const UI::WidgetPtr parUIElement)
{

    const UI::WidgetPlacement& placement = parUIElement->Placement();
    FBuffer.PushContext(placement.FPositionInPixels);
    DrawUIElements(parUIElement);
    FBuffer.PopContext();
}

void UIRenderer::DrawUIElements(const UI::WidgetPtr parUIElement)
{
    DrawThisElement(parUIElement);

    foreachitem(child, parUIElement->Children()) { DrawThisElement(child); }
}

void UIRenderer::DrawThisElement(const UI::WidgetPtr parUIElement)
{
    VertexDataStream* stream = nullptr;
    std::vector<u32>* indices = nullptr;

    FBuffer.GetCurrentStreams(FUIVertexColorMaterial, stream, indices);

    const UI::WidgetPlacement& placement = parUIElement->Placement();
    glm::vec2 pos = glm::xy(placement.FPositionInPixels);
    glm::vec2 size = glm::zw(placement.FPositionInPixels);
    u32 cl = std::dynamic_pointer_cast<UI::TestUIWidget>(parUIElement)->Color;
    const u32 indexOffset = (u32)stream->GetSize();

    stream->PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(pos, 0.f));
    stream->PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, cl);
    stream->Advance();

    stream->PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(pos + glm::vec2(size.x, 0.f), 0.f));
    stream->PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, cl);
    stream->Advance();

    stream->PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(pos + size, 0.f));
    stream->PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, cl);
    stream->Advance();

    stream->PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(pos + glm::vec2(0.f, size.y), 0.f));
    stream->PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, cl);
    stream->Advance();

    indices->push_back(indexOffset + 0);
    indices->push_back(indexOffset + 1);
    indices->push_back(indexOffset + 2);

    indices->push_back(indexOffset + 0);
    indices->push_back(indexOffset + 2);
    indices->push_back(indexOffset + 3);
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
