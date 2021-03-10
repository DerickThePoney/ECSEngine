#include "stdafx.h"

#include "Profiling.h"

namespace ECSEngine
{
namespace Profiling
{
#ifdef ENABLE_PROFILING
Remotery* rmt = nullptr;
#endif

void StartProfiler()
{
#ifdef ENABLE_PROFILING
    rmt_CreateGlobalInstance(&rmt);
    AssertRelease(rmt != nullptr);
#endif
}

void EndProfiler()
{
#ifdef ENABLE_PROFILING
    rmt_DestroyGlobalInstance(rmt);
#endif
}
} // namespace Profiling
} // namespace ECSEngine
