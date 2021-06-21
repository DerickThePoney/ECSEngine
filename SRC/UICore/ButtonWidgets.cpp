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

SimpleButton::~SimpleButton()
{
    FLabelWidget = nullptr;
}

const std::string& SimpleButton::Text() const
{
    if (FLabelWidget != nullptr)
    {
        std::shared_ptr<SimpleLabel> labelAsSimpleLabel = std::dynamic_pointer_cast<SimpleLabel>(FLabelWidget);
        AssertRelease(labelAsSimpleLabel != nullptr);
        return labelAsSimpleLabel->Text();
    }

    static std::string emptyString = "";
    return emptyString;
}

void SimpleButton::SetText(const std::string& parValue)
{
    if (FLabelWidget != nullptr)
    {
        std::shared_ptr<SimpleLabel> labelAsSimpleLabel = std::dynamic_pointer_cast<SimpleLabel>(FLabelWidget);
        AssertRelease(labelAsSimpleLabel != nullptr);
        labelAsSimpleLabel->SetText(parValue);
    }
}

void SimpleButton::VirtualOnDraw(Rendering::UICommandBuffer& parBuffer)
{
    Widget::VirtualOnDraw(parBuffer);

    const UI::WidgetPlacement& placement = Placement();
    glm::vec2 pos = glm::xy(placement.FPositionInPixels);
    glm::vec2 size = glm::zw(placement.FPositionInPixels);

    FBackgroundDrawer.Draw(parBuffer, pos, size);
}

} // namespace UI
} // namespace ECSEngine