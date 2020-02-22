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
    FRunning = true;
    FStart = std::chrono::high_resolution_clock::now();
}

const float Timer::Stop()
{
    FEnd = std::chrono::high_resolution_clock::now();
    FRunning = false;
    return ElapsedTimeInSeconds(FEnd);
}

const float Timer::ElapsedTimeInSeconds(const std::chrono::high_resolution_clock::time_point& parTo) const
{
    using fpSeconds = std::chrono::duration<float, std::chrono::seconds::period>;
    static_assert(std::chrono::treat_as_floating_point<fpSeconds::rep>::value, "Rep required to be floating point");
    return fpSeconds(parTo - FStart).count();
}

const float Timer::ElapsedTimeInMilliseconds(const std::chrono::high_resolution_clock::time_point& parTo) const
{
    using fpMilli = std::chrono::duration<float, std::chrono::milliseconds::period>;
    static_assert(std::chrono::treat_as_floating_point<fpMilli::rep>::value, "Rep required to be floating point");
    return fpMilli(parTo - FStart).count();
}

const float Timer::ElapsedTimeInMicroseconds(const std::chrono::high_resolution_clock::time_point& parTo) const
{
    using fpMicro = std::chrono::duration<float, std::chrono::microseconds::period>;
    static_assert(std::chrono::treat_as_floating_point<fpMicro::rep>::value, "Rep required to be floating point");
    return fpMicro(parTo - FStart).count();
}

const float Timer::ElapsedTimeInNanoseconds(const std::chrono::high_resolution_clock::time_point& parTo) const
{
    using fpNano = std::chrono::duration<float, std::chrono::nanoseconds::period>;
    static_assert(std::chrono::treat_as_floating_point<fpNano::rep>::value, "Rep required to be floating point");
    return fpNano(parTo - FStart).count();
}

TScopedTimer::TScopedTimer(const char* name)
    : FName(name)
{
    FTimer.Start();
}

TScopedTimer::~TScopedTimer()
{
    const float elapsed = FTimer.Stop();
    std::cout << "Timer " << FName << " : " << elapsed << " sec\n";
}

} // namespace ECSEngine
