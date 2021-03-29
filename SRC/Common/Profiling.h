#pragma once

#ifdef ENABLE_PROFILING
#include "Remotery.h"

#define SCOPED_PROFILE(NAME) rmt_ScopedCPUSample(NAME, 0);
#define SCOPED_PROFILE_CLASS(CLASS, METHOD) rmt_ScopedCPUSample(CLASS##_##METHOD, 0);

#else

#define SCOPED_PROFILE(NAME)
#define SCOPED_PROFILE_CLASS(CLASS, METHOD)
#endif

namespace ECSEngine
{
namespace Profiling
{
void StartProfiler();
void EndProfiler();
} // namespace Profiling
} // namespace ECSEngine
