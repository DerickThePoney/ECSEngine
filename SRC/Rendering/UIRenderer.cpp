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
#include "UICore/ButtonWidgets.h"
#include "UICore/Font.h"
#include "UICore/LabelWidget.h"
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
    std::vector<std::unique_ptr<UI::PanelWidgetDescriptor>> FPanels;
    std::vector<UI::WidgetPtr> FUIElementsForTest;

    UICommandBuffer FBuffer;

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
    using namespace UI;

    std::unique_ptr<PanelWidgetDescriptor> panelDesc = std::make_unique<PanelWidgetDescriptor>();
    WidgetPlacement& panelPlacement = panelDesc->InitialPlacement();
    panelPlacement.FSizeType = WidgetSizeType::ABSOLUTE_PIXEL;
    panelPlacement.FSize = glm::vec2(400.0f, 500.f);
    panelPlacement.FPositionFromAnchor = glm::vec2(200.f);

    panelDesc->SetColor(ColorUtils::ConvertToU32(glm::vec4(1.f, 0.f, 0.f, 0.5f)));

    std::unique_ptr<LabelWidgetDescriptor> labelDesc = std::make_unique<LabelWidgetDescriptor>();
    WidgetPlacement& labelPlacement = labelDesc->InitialPlacement();
    labelPlacement.FPositionFromAnchor = glm::vec2(0.f);
    labelDesc->SetTextToDraw("Test string to \nrasterize");
    labelDesc->SetTextColor(ColorUtils::ConvertToU32(glm::vec4(0.f, 0.f, 1.f, 1.f)));
    labelDesc->SetFont("fonts\\kenvector_future.ttf", 14.f);

    std::unique_ptr<ButtonWidgetDescriptor> buttonDesc = std::make_unique<ButtonWidgetDescriptor>();
    WidgetPlacement& buttonPlacement = buttonDesc->InitialPlacement();
    buttonPlacement.FPositionFromAnchor = glm::vec2(50.f, 50.f);
    buttonPlacement.FSize = glm::vec2(100.0f, 40.f);
    buttonDesc->SetColor(ColorUtils::ConvertToU32(glm::vec4(0.f, 1.f, 0.f, 1.f))); // Vert
    LabelWidgetDescriptor& bLab = buttonDesc->LabelWidgetDescription();
    WidgetPlacement& bLabelPlacement = bLab.InitialPlacement();
    bLabelPlacement.FPositionFromAnchor = glm::vec2(0.f);
    bLabelPlacement.FParentAnchor = WidgetParentAnchor::CENTER_LEFT;
    bLab.SetTextToDraw("Button String");
    bLab.SetTextColor(ColorUtils::ConvertToU32(glm::vec4(1.f, 0.f, 0.f, 1.f)));
    bLab.SetFont("fonts\\kenvector_future.ttf", 14.f);

    panelDesc->Children().push_back(std::move(labelDesc));
    panelDesc->Children().push_back(std::move(buttonDesc));

    FPanels.push_back(std::move(panelDesc));
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
