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
    return WidgetPtr(new SimplePanel(this, InitialPlacement()));
}

bool PanelWidgetDescriptor::VirtualDrawEditor()
{
    if (ImGui::CollapsingHeader("Panel widget"))
    {
        WidgetDescriptor::VirtualDrawEditor();
        ImGui::Separator();

        glm::vec4 color = ColorUtils::ConvertToFVEC4(FColor);
        EDITOR_PROPERTY_COLOR("Panel color", color);
        FColor = ColorUtils::ConvertToU32(color);
        return true;
    }
    return false;
}

IMPLEMENT_POOL_ALLOCATED(SimplePanel);
SimplePanel::SimplePanel(const PanelWidgetDescriptor* parDescriptor, const WidgetPlacement& parPlacement)
    : Widget(parPlacement)
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
    foreachitem(child, Children()) { child->Draw(parBuffer); }
    parBuffer.PopContext();
}

} // namespace UI
} // namespace ECSEngine