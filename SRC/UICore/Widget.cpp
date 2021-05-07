#include "stdafx.h"

#include "Widget.h"

namespace ECSEngine
{
namespace UI
{

void Widget::SetParent(WidgetPtr parNewParent)
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
}

void Widget::RemoveChild(WidgetPtr parOldChild)
{
    auto it = std::find(FChildren.begin(), FChildren.end(), parOldChild);
    AlwaysCheckedAssert(it != FChildren.end());
    if (it != FChildren.end())
        FChildren.erase(it);
}

} // namespace UI
} // namespace ECSEngine