#include "stdafx.h"

#include "PanelWidgets.h"

#include "Application/PropertyDrawer.h"
#include "Common/ColorUtils.h"
#include "Common/MeshStreamingData.h"
#include "RenderingCore/UICommandBuffer.h"
#include "UIMaterialsHelpers.h"

namespace ECSEngine
{
namespace UI
{

WidgetPtr PanelWidgetDescriptor::CreateThisWidget() const
{
    return WidgetPtr(new SimplePanel(FName, this, InitialPlacement()));
}

bool PanelWidgetDescriptor::VirtualDrawEditor()
{
    if (WidgetDescriptor::VirtualDrawEditor())
    {
        glm::vec4 color = ColorUtils::ConvertToFVEC4(FColor);
        EDITOR_PROPERTY_COLOR("Panel color", color);
        FColor = ColorUtils::ConvertToU32(color);
        return true;
    }
    return false;
}

IMPLEMENT_POOL_ALLOCATED(SimplePanel);
SimplePanel::SimplePanel(const std::string& parName, const PanelWidgetDescriptor* parDescriptor, const WidgetPlacement& parPlacement)
    : Widget(parName, parPlacement)
    , FDescriptor(parDescriptor)
{
    FBackgroundDrawer.FColor = FDescriptor->Color();
    FBackgroundDrawer.FMaterialInstanceHandle = GetVertexColorMaterial();
}

void SimplePanel::VirtualOnDraw(Rendering::UICommandBuffer& parBuffer)
{
    Widget::VirtualOnDraw(parBuffer);

    const UI::WidgetPlacement& placement = Placement();
    glm::vec2 pos = glm::xy(placement.FPositionInPixels);
    glm::vec2 size = glm::zw(placement.FPositionInPixels);

    FBackgroundDrawer.Draw(parBuffer, pos, size);

    parBuffer.PushContext(placement.FPositionInPixels);
}

void SimplePanel::VirtualPostDraw(Rendering::UICommandBuffer& parBuffer)
{
    Widget::VirtualPostDraw(parBuffer);
    parBuffer.PopContext();
}

} // namespace UI
} // namespace ECSEngine