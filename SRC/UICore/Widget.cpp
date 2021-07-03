#include "stdafx.h"

#include "Widget.h"

namespace ECSEngine
{
namespace UI
{

Widget::Widget(const std::string& parName, const WidgetPlacement& parInitialPlacement)
    : FPlacement(parInitialPlacement)
{
}

Widget::~Widget()
{
    FChildren.clear();
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

    VirtualPreDraw(parBuffer);
    AlwaysCheckedAssertMsg(FVirtualPreDrawCalled, "A call to Widget::VirtualOnDraw was forgotten");

    VirtualOnDraw(parBuffer);
    AlwaysCheckedAssertMsg(FVirtualOnDrawCalled, "A call to Widget::VirtualPreDraw was forgotten");

    foreachitem(child, FChildren) { child->Draw(parBuffer); }

    VirtualPostDraw(parBuffer);
    AlwaysCheckedAssertMsg(FVirtualPostDrawCalled, "A call to Widget::VirtualPostDraw was forgotten");
}

void Widget::VirtualOnDraw(Rendering::UICommandBuffer& parBuffer)
{
    OnlyWithAssertions(FVirtualOnDrawCalled = true);
}

void Widget::VirtualPreDraw(Rendering::UICommandBuffer& parBuffer)
{
    OnlyWithAssertions(FVirtualPreDrawCalled = true);
}

void Widget::VirtualPostDraw(Rendering::UICommandBuffer& parBuffer)
{
    OnlyWithAssertions(FVirtualPostDrawCalled = true);
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
    if (ImGui::CollapsingHeader(FName.c_str()))
    {
        static char buf[1024];
        strcpy(buf, FName.c_str());
        ImGui::InputText("Name", buf, 1024);
        FName = buf;
        FInitialPlacement.DrawEditor();
        return true;
    }
    return false;
}

} // namespace UI
} // namespace ECSEngine