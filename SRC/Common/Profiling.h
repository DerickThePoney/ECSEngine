#pragma once

//#ifdef ENABLE_PROFILING
//#include "Remotery.h"
//
//#define SCOPED_PROFILE(NAME) rmt_ScopedCPUSample(NAME, 0);
//#define SCOPED_PROFILE_CLASS(CLASS, METHOD) rmt_ScopedCPUSample(CLASS##_##METHOD, 0);
//
//#else
//
//#define SCOPED_PROFILE(NAME)
//#define SCOPED_PROFILE_CLASS(CLASS, METHOD)
//#endif
#include "Macros.h"
#ifdef ENABLE_PROFILING
#undef max
#include "Tracy.hpp"
// void ScopedProfile(const char*);

#define SCOPED_PROFILE(NAME) ZoneScopedN(#NAME);
#define SCOPED_PROFILE_CLASS(CLASS, METHOD) ZoneScoped;
#define SCOPED_PROFILE_SIMPLE ZoneScoped;
#define FRAME_END FrameMark;

#else

#define SCOPED_PROFILE(NAME)
#define SCOPED_PROFILE_CLASS(CLASS, METHOD)
#define SCOPED_PROFILE_SIMPLE
#define FRAME_END
#endif

namespace ECSEngine
{
namespace Profiling
{
void StartProfiler();
void EndProfiler();
} // namespace Profiling
} // namespace ECSEngine
