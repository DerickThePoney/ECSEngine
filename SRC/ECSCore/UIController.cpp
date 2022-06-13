#include "stdafx.h"

#include "UIController.h"

namespace ECSEngine
{
namespace UI
{

UIController::~UIController()
{
}

void UIController::Init()
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualInitCalled = false;
#endif
    VirtualInit();
    AlwaysCheckedAssert(FVirtualInitCalled);
}

void UIController::Update()
{
    if (!FShow)
        return;

#ifdef PERFORM_SECURITY_CHECKS
    FVirtualUpdateCalled = false;
#endif
    VirtualUpdate();
    AlwaysCheckedAssert(FVirtualUpdateCalled);
}

void UIController::Destroy()
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualDestroyCalled = false;
#endif
    VirtualDestroy();
    AlwaysCheckedAssert(FVirtualDestroyCalled);
}

void UIController::VirtualInit()
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualInitCalled = true;
#endif
}

void UIController::VirtualUpdate()
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualUpdateCalled = true;
#endif
}

void UIController::VirtualDestroy()
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualDestroyCalled = true;
#endif
}

} // namespace UI
} // namespace ECSEngine
