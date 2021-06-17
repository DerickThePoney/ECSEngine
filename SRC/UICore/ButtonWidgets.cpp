#include "stdafx.h"

#include "ButtonWidgets.h"

#include "UIMaterialsHelpers.h"

namespace ECSEngine
{
namespace UI
{

WidgetPtr ButtonWidgetDescriptor::CreateThisWidget() const
{
    return WidgetPtr(new SimpleButton(this));
}

bool ButtonWidgetDescriptor::VirtualDrawEditor()
{
    if (ImGui::CollapsingHeader("Button widget"))
    {
        WidgetDescriptor::VirtualDrawEditor();
        return true;
    }
    return false;
}

IMPLEMENT_POOL_ALLOCATED(SimpleButton);
SimpleButton::SimpleButton(const ButtonWidgetDescriptor* parDescriptor)
    : Widget(parDescriptor->InitialPlacement())
    , FColor(parDescriptor->Color())
{
    FBackgroundDrawer.FColor = parDescriptor->Color();
    FBackgroundDrawer.FMaterialInstanceHandle = GetVertexColorMaterial();
    FLabelWidget = parDescriptor->LabelWidgetDescription().CreateWidgetAndHierarchy();
    AddChild(FLabelWidget);
}

void SimpleButton::VirtualOnDraw(Rendering::UICommandBuffer& parBuffer)
{
    Widget::VirtualOnDraw(parBuffer);

    const UI::WidgetPlacement& placement = Placement();
    glm::vec2 pos = glm::xy(placement.FPositionInPixels);
    glm::vec2 size = glm::zw(placement.FPositionInPixels);

    FBackgroundDrawer.Draw(parBuffer, pos, size);

    FLabelWidget->Draw(parBuffer);
}

} // namespace UI
} // namespace ECSEngine