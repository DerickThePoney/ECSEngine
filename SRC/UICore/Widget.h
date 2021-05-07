#pragma once
#include "Common/MemoryView.h"

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

private:
    WidgetPtr FParent = nullptr;
    std::vector<WidgetPtr> FChildren;
};
} // namespace UI
} // namespace ECSEngine