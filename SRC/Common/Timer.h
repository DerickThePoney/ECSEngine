#pragma once

namespace ECSEngine
{

namespace ETimePeriod
{
enum Type
{
    SECONDS,
    MILLISECONDS,
    MICROSECONDS,
    NANOSECONDS
};
}

class Timer
{
public:
    Timer(bool parStartImmediately = false);
    ~Timer();

    void Start();
    const float Stop();

    template<enum ETimePeriod::Type Period = ETimePeriod::SECONDS>
    const float ElapsedTime() const
    {
        AlwaysCheckedAssert(!FRunning);
        switch (Period)
        {
        case ECSEngine::ETimePeriod::SECONDS:
            return ElapsedTimeInSeconds(FEnd);
            break;
        case ECSEngine::ETimePeriod::MILLISECONDS:
            return ElapsedTimeInMilliseconds(FEnd);
            break;
        case ECSEngine::ETimePeriod::MICROSECONDS:
            return ElapsedTimeInMicroseconds(FEnd);
            break;
        case ECSEngine::ETimePeriod::NANOSECONDS:
            return ElapsedTimeInNanoseconds(FEnd);
            break;
        default:
            AssertNotReached();
        }

        return -1.f;
    }

    template<enum ETimePeriod::Type Period = ETimePeriod::SECONDS>
    const float PeekDurationSinceStart() const
    {
        AlwaysCheckedAssert(FRunning);
        const std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        switch (Period)
        {
        case ECSEngine::ETimePeriod::SECONDS:
            return ElapsedTimeInSeconds(now);
            break;
        case ECSEngine::ETimePeriod::MILLISECONDS:
            return ElapsedTimeInMilliseconds(now);
            break;
        case ECSEngine::ETimePeriod::MICROSECONDS:
            return ElapsedTimeInMicroseconds(now);
            break;
        case ECSEngine::ETimePeriod::NANOSECONDS:
            return ElapsedTimeInNanoseconds(now);
            break;
        default:
            AssertNotReached();
        }

        return -1.f;
    }

    bool Running() const { return FRunning; }

private:
    const float ElapsedTimeInSeconds(const std::chrono::high_resolution_clock::time_point& parTo) const;
    const float ElapsedTimeInMilliseconds(const std::chrono::high_resolution_clock::time_point& parTo) const;
    const float ElapsedTimeInMicroseconds(const std::chrono::high_resolution_clock::time_point& parTo) const;
    const float ElapsedTimeInNanoseconds(const std::chrono::high_resolution_clock::time_point& parTo) const;

private:
    std::chrono::high_resolution_clock::time_point FStart;
    std::chrono::high_resolution_clock::time_point FEnd;

    bool FRunning;
};

class TScopedTimer
{
public:
    TScopedTimer(const char* name);
    ~TScopedTimer();

private:
    std::string FName;
    Timer FTimer;
};
} // namespace ECSEngine
