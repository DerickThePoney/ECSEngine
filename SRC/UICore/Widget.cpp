#include "stdafx.h"

#include "Widget.h"

namespace ECSEngine
{
namespace UI
{

Widget::~Widget()
{
}

void Widget::SetParent(Widget* parNewParent)
{
    FParent = parNewParent;
}

void Widget::AddChild(WidgetPtr parNewChild)
{
#ifdef ENABLE_SECURITY_CHECKS
    auto it = std::find(FChildren.begin(), FChildren.end(), parNewChild);
    AlwaysCheckedAssert(it == FChildren.end());
#endif

    FChildren.push_back(parNewChild);
    parNewChild->SetParent(this);
}

void Widget::RemoveChild(WidgetPtr parOldChild)
{
    auto it = std::find(FChildren.begin(), FChildren.end(), parOldChild);
    AlwaysCheckedAssert(it != FChildren.end());
    if (it != FChildren.end())
    {
        (*it)->SetParent(nullptr);
        FChildren.erase(it);
    }
}

void Widget::UpdatePlacement(const WidgetScaler* parScaler)
{
    FPlacement.UpdatePlacementIFN(parScaler, FParent);
    foreachitem(child, FChildren) { child->UpdatePlacement(parScaler); }
}

} // namespace UI
} // namespace ECSEngine