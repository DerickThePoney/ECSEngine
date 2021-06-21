#pragma once
#include "Common/PoolAllocator.h"
#include "UIDrawer.h"
#include "Widget.h"

namespace ECSEngine
{
namespace UI
{
class PanelWidgetDescriptor : public WidgetDescriptor
{
public:
    virtual ~PanelWidgetDescriptor() = default;
    u32 Color() const { return FColor; }

    void SetColor(const u32 parColor) { FColor = parColor; }

    SERIALIZE()
    {
        ar(cereal::base_class<WidgetDescriptor>(this));
        PROPERTYFIELD(Color, 0xFFFFFFFF);
    }

protected:
    virtual WidgetPtr CreateThisWidget() const override;
    virtual bool VirtualDrawEditor() override;

private:
    u32 FColor;
};

class SimplePanel : public Widget
{
    DECLARE_POOL_ALLOCATED(SimplePanel);

public:
    SimplePanel(const PanelWidgetDescriptor* parDescriptor, const WidgetPlacement& parPlacement);

protected:
    virtual void VirtualOnDraw(Rendering::UICommandBuffer& parBuffer) override;
    virtual void VirtualPostDraw(Rendering::UICommandBuffer& parBuffer) override;

private:
    const PanelWidgetDescriptor* FDescriptor = nullptr;
    UIBackgroundDrawer FBackgroundDrawer;
};
} // namespace UI
} // namespace ECSEngine