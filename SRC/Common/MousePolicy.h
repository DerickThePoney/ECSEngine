#pragma once
#include "MousePolicyType.h"

namespace ECSEngine
{
class IMousePolicy
{
public:
    bool IsActivated() const { return FIsActivated; }

    void Activate();
    void Deactivate();
    void Update();

    virtual MousePolicyType::Type MousePolicyType() const = 0;

protected:
    virtual void VirtualActivate();
    virtual void VirtualDeactivate();
    virtual void VirtualUpdate();

private:
    bool FIsActivated = false;
#ifdef ENABLE_SECURITY_CHECKS
    bool FVirtualActivateCalled = false;
    bool FVirtualDeactivateCalled = false;
    bool FVirtualUpdateCalled = false;
#endif
};

#define MOUSE_POLICY_HEADER(CLASS, PARENT_CLASS, ENUMTYPE)                                                                                                                         \
    using parent_type = PARENT_CLASS;                                                                                                                                              \
                                                                                                                                                                                   \
public:                                                                                                                                                                            \
    virtual MousePolicyType::Type MousePolicyType() const override { return ENUMTYPE; }                                                                                            \
                                                                                                                                                                                   \
private:
} // namespace ECSEngine
