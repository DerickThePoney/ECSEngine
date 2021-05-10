#pragma once
#include "Common/MemoryView.h"
#include "WidgetPlacement.h"

namespace ECSEngine
{
namespace UI
{
class Widget;
using WidgetPtr = std::shared_ptr<Widget>;

class Widget
{
public:
    virtual ~Widget();
    void SetParent(Widget* parNewParent);
    void AddChild(WidgetPtr parNewChild);
    void RemoveChild(WidgetPtr parOldChild);

    const Widget* Parent() const { return FParent; }
    MemoryView<const WidgetPtr> Children() const { return MemoryView<const WidgetPtr>(FChildren.data(), FChildren.size()); }

    const WidgetPlacement& Placement() const { return FPlacement; }
    WidgetPlacement& Placement() { return FPlacement; }

    virtual void UpdatePlacement(const WidgetScaler* parScaler);

private:
    Widget* FParent = nullptr;
    std::vector<WidgetPtr> FChildren;

    WidgetPlacement FPlacement;
};

class TestUIWidget : public Widget
{
public:
    u32 Color = 0xFFFFFFFF;
};
} // namespace UI
} // namespace ECSEngine