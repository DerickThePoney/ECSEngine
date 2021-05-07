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
    void SetParent(WidgetPtr parNewParent);
    void AddChild(WidgetPtr parNewChild);
    void RemoveChild(WidgetPtr parOldChild);

    const Widget* Parent() const { return FParent.get(); }
    MemoryView<const WidgetPtr> Children() const { return MemoryView<const WidgetPtr>(FChildren.data(), FChildren.size()); }

    const WidgetPlacement& Placement() const { return FPlacement; }
    WidgetPlacement& Placement() { return FPlacement; }

private:
    WidgetPtr FParent = nullptr;
    std::vector<WidgetPtr> FChildren;

    WidgetPlacement FPlacement;
};
} // namespace UI
} // namespace ECSEngine