#pragma once
#include "Common/PoolAllocator.h"
#include "LabelWidget.h"
#include "UIDrawer.h"
#include "Widget.h"

namespace ECSEngine
{
namespace UI
{
class ButtonWidgetDescriptor : public WidgetDescriptor
{
public:
    virtual ~ButtonWidgetDescriptor() = default;

    u32 Color() const { return FColor; }
    void SetColor(u32 parValue) { FColor = parValue; }

    const LabelWidgetDescriptor& LabelWidgetDescription() const { return FLabelWidget; }

    SERIALIZE()
    {
        ar(cereal::base_class<WidgetDescriptor>(this));
        PROPERTYFIELD(LabelWidget, LabelWidgetDescriptor());
        PROPERTYFIELD(Color, 0xFFFFFFFF);
    }

protected:
    virtual WidgetPtr CreateThisWidget() const override;
    virtual bool VirtualDrawEditor() override;

private:
    LabelWidgetDescriptor FLabelWidget;
    u32 FColor = 0xFFFFFFFF;
};

class SimpleButton : public Widget
{
    DECLARE_POOL_ALLOCATED(SimpleButton);

public:
    SimpleButton(const ButtonWidgetDescriptor* parDescriptor);

    u32 Color() const { return FColor; }
    void SetColor(u32 parValue) { FColor = parValue; }

    const std::string& Text() const;
    void SetText(const std::string& parValue);

protected:
    virtual void VirtualOnDraw(Rendering::UICommandBuffer& parBuffer);

private:
    UIBackgroundDrawer FBackgroundDrawer;

    WidgetPtr FLabelWidget;
    u32 FColor = 0xFFFFFFFF;
};
} // namespace UI
} // namespace ECSEngine