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

    std::unique_ptr<PanelWidgetDescriptor> panelDesc = std::make_unique<PanelWidgetDescriptor>();
    WidgetPlacement& placement = panelDesc->InitialPlacement();
    placement.FSizeType = WidgetSizeType::ABSOLUTE_RELATIVE;
    placement.FSize = glm::vec2(1.f, 0.5f);

    panelDesc->SetColor(ColorUtils::ConvertToU32(glm::vec4(1.f, 0.f, 0.f, 0.5f)));

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
