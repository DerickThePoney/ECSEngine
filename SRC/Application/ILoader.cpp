#include "stdafx.h"

#include "ILoader.h"

namespace ECSEngine
{

ILoader::ILoader()
{
}

ILoader::~ILoader()
{
}

bool ILoader::Initialise()
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualInitialisedCalled = false;
#endif // DEBUG

    bool res = VirtualInitialise();

    AlwaysCheckedAssertMsg(res, FLoaderName.c_str());

    FHasBeenInitialised = res;

    AlwaysCheckedAssert(FVirtualInitialisedCalled);
    return res;
}

void ILoader::Shutdown()
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualShutdownCalled = false;
#endif // DEBUG

    VirtualShutdown();

    AlwaysCheckedAssert(FVirtualShutdownCalled);
}

bool ILoader::VirtualInitialise()
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualInitialisedCalled = true;
#endif
    return true;
}

void ILoader::VirtualShutdown()
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualShutdownCalled = true;
#endif //
}

} // namespace ECSEngine

CEREAL_REGISTER_TYPE(ECSEngine::ILoader);