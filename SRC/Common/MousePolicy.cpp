#include "stdafx.h"

#include "MousePolicy.h"

namespace ECSEngine
{
void IMousePolicy::Activate()
{
    AlwaysCheckedAssert(!FIsActivated);
#ifdef ENABLE_SECURITY_CHECKS
    FVirtualActivateCalled = false;
#endif
    VirtualActivate();
    AlwaysCheckedAssert(FVirtualActivateCalled);
    AlwaysCheckedAssert(FIsActivated);
}

void IMousePolicy::Deactivate()
{
    AlwaysCheckedAssert(FIsActivated);
#ifdef ENABLE_SECURITY_CHECKS
    FVirtualDeactivateCalled = false;
#endif
    VirtualDeactivate();
    AlwaysCheckedAssert(FVirtualDeactivateCalled);
    AlwaysCheckedAssert(!FIsActivated);
}

void IMousePolicy::Update()
{
    AlwaysCheckedAssert(FIsActivated);
    if (!FIsActivated)
        return;
#ifdef ENABLE_SECURITY_CHECKS
    FVirtualUpdateCalled = false;
#endif
    VirtualUpdate();
    AlwaysCheckedAssert(FVirtualUpdateCalled);
}

void IMousePolicy::VirtualActivate()
{
#ifdef ENABLE_SECURITY_CHECKS
    FVirtualActivateCalled = true;
#endif // ENABLE_SECURITY_CHECKS
    FIsActivated = true;
}

void IMousePolicy::VirtualDeactivate()
{
#ifdef ENABLE_SECURITY_CHECKS
    FVirtualDeactivateCalled = true;
#endif // ENABLE_SECURITY_CHECKS
    FIsActivated = false;
}

void IMousePolicy::VirtualUpdate()
{
#ifdef ENABLE_SECURITY_CHECKS
    FVirtualUpdateCalled = true;
#endif
}

} // namespace ECSEngine
