#include "stdafx.h"

#include "Timer.h"
namespace ECSEngine
{
Timer::Timer(bool parStartImmediately /*= false*/)
    : FStart()
    , FEnd()
#ifdef PERFORM_SECURITY_CHECKS
    , FRunning(false)
#endif
{
    if (parStartImmediately)
        Start();
}

Timer::~Timer()
{
}

void Timer::Start()
{
    AlwaysCheckedAssert(!FRunning);
#ifdef PERFORM_SECURITY_CHECKS
    FRunning = true;
#endif
    FStart = std::chrono::high_resolution_clock::now();
}

const float Timer::Stop()
{
    FEnd = std::chrono::high_resolution_clock::now();
#ifdef PERFORM_SECURITY_CHECKS
    FRunning = false;
#endif
    return ElapsedTimeInSeconds(FEnd);
}

const float Timer::ElapsedTimeInSeconds(const std::chrono::high_resolution_clock::time_point& parTo) const
{
    return (float)(parTo - FStart).count() / 1000000000.f;
}

const float Timer::ElapsedTimeInMilliseconds(const std::chrono::high_resolution_clock::time_point& parTo) const
{
    return (float)(parTo - FStart).count() / 1000000.f;
}

const float Timer::ElapsedTimeInMicroseconds(const std::chrono::high_resolution_clock::time_point& parTo) const
{
    return (float)(parTo - FStart).count() / 1000.f;
}

const float Timer::ElapsedTimeInNanoseconds(const std::chrono::high_resolution_clock::time_point& parTo) const
{
    return (float)(parTo - FStart).count();
}

} // namespace ECSEngine
