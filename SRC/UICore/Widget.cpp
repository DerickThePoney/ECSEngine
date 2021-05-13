#include "stdafx.h"

#include "Widget.h"

namespace ECSEngine
{
namespace UI
{

Widget::Widget(const WidgetPlacement& parInitialPlacement)
    : FPlacement(parInitialPlacement)
{
}

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

void Widget::Draw(Rendering::UICommandBuffer& parBuffer)
{
    OnlyWithAssertions(FVirtualOnDrawCalled = false);

    VirtualOnDraw(parBuffer);

    AlwaysCheckedAssertMsg(FVirtualOnDrawCalled, "A call to Widget::VirtualOnDraw was forgotten");
}

void Widget::VirtualOnDraw(Rendering::UICommandBuffer& parBuffer)
{
    OnlyWithAssertions(FVirtualOnDrawCalled = true);
}

WidgetPtr WidgetDescriptor::CreateWidgetAndHierarchy() const
{
    WidgetPtr thisWidget = CreateThisWidget();

    foreachitemconst(child, FChildren)
    {
        WidgetPtr c = child->CreateWidgetAndHierarchy();
        c->SetParent(thisWidget.get());
        thisWidget->AddChild(c);
    }

    return thisWidget;
}

void WidgetDescriptor::DrawEditor()
{
    if (VirtualDrawEditor())
    {
        ImGui::Indent();
        foreachitem(child, FChildren) { child->DrawEditor(); }
        ImGui::Unindent();
    }
}

bool WidgetDescriptor::VirtualDrawEditor()
{
    FInitialPlacement.DrawEditor();
    return true;
}

} // namespace UI
} // namespace ECSEngine