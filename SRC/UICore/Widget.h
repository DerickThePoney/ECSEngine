#pragma once
#include "Common/MemoryView.h"
#include "WidgetPlacement.h"

namespace ECSEngine
{
namespace Rendering
{
class UICommandBuffer;
}

namespace UI
{
class Widget;
using WidgetPtr = std::shared_ptr<Widget>;

class WidgetDescriptor
{
public:
    virtual ~WidgetDescriptor() = default;

    WidgetPtr CreateWidgetAndHierarchy() const;
    void DrawEditor();

    std::vector<std::unique_ptr<WidgetDescriptor>>& Children() { return FChildren; }
    const WidgetPlacement& InitialPlacement() const { return FInitialPlacement; }
    WidgetPlacement& InitialPlacement() { return FInitialPlacement; }

    SERIALIZE() { PROPERTYFIELD(InitialPlacement, WidgetPlacement()); }

protected:
    virtual WidgetPtr CreateThisWidget() const = 0;
    virtual bool VirtualDrawEditor();

protected:
    std::string FName;

private:
    std::vector<std::unique_ptr<WidgetDescriptor>> FChildren;
    WidgetPlacement FInitialPlacement;
};

class Widget
{
public:
    Widget(const std::string& parName, const WidgetPlacement& parInitialPlacement);
    virtual ~Widget();
    void SetParent(Widget* parNewParent);
    void AddChild(WidgetPtr parNewChild);
    void RemoveChild(WidgetPtr parOldChild);

    const Widget* Parent() const { return FParent; }
    MemoryView<const WidgetPtr> Children() const { return MemoryView<const WidgetPtr>(FChildren.data(), FChildren.size()); }

    const WidgetPlacement& Placement() const { return FPlacement; }
    WidgetPlacement& Placement() { return FPlacement; }

    virtual void UpdatePlacement(const WidgetScaler* parScaler);

    void Draw(Rendering::UICommandBuffer& parBuffer);

protected:
    virtual void VirtualOnDraw(Rendering::UICommandBuffer& parBuffer);
    virtual void VirtualPreDraw(Rendering::UICommandBuffer& parBuffer);
    virtual void VirtualPostDraw(Rendering::UICommandBuffer& parBuffer);

private:
    Widget* FParent = nullptr;
    std::vector<WidgetPtr> FChildren;

    WidgetPlacement FPlacement;
    std::string FWidgetName = "";

#ifdef ENABLE_SECURITY_CHECKS
    bool FVirtualOnDrawCalled = false;
    bool FVirtualPreDrawCalled = false;
    bool FVirtualPostDrawCalled = false;
#endif
};
} // namespace UI
} // namespace ECSEngine