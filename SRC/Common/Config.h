#pragma once

#define DONOTHING (void)(0)

#ifdef PERFORM_SECURITY_CHECKS
#define ENABLE_DEBUG_PARAMETERS
#define ENABLE_SECURITY_CHECKS
#define WITH_VISUAL_DEBUG
#endif

#ifdef PROFILE_CODE
#define ENABLE_PROFILING
#endif

#ifdef PROFILE
#define COMPILE_PROFILE
#endif

#ifdef FINAL
#define COMPILE_FINAL
#endif

#define FORCEINLINE inline __forceinline