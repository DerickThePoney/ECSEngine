#include "stdafx.h"

#include "TimeManager.h"

#include "Logger.h"
#include "Singleton.h"
#include "Timer.h"

namespace ECSEngine
{
constexpr float GameplayTickSize = 0.1f;

class TimeManagerImpl : public Singleton<TimeManagerImpl>
{
public:
    void Start();
    void NewFrame();
    void NewGameplayTick();
    void End();

    float FrameDeltaTime() const { return FFrameDeltaTime; }
    float FrameStartTime() const { return FFrameStartTime; }
    float DurationSinceStartRealTime() const { return FGlobalTimer.PeekDurationSinceStart<ETimePeriod::SECONDS>(); }
    u32 GetFrameNumber() const { return FFrameNumber; }
    u32 CurrentGameplayTick() const { return FCurrentGameplayTick; }
    float CurrentGameplayTime() const { return FCurrentGameplayTick * GameplayTickSize; }

private:
    Timer FGlobalTimer;
    Timer FFrameDurationTimer;

    float FFrameStartTime = 0.f;
    float FFrameDeltaTime = 0.f;
    u32 FFrameNumber = 0;
    u32 FCurrentGameplayTick = 0;
};

void TimeManagerImpl::Start()
{
    FGlobalTimer.Start();
}

void TimeManagerImpl::NewFrame()
{
    if (!FGlobalTimer.Running())
        Start();

    if (FFrameNumber != 0)
    {
        FFrameDurationTimer.Stop();
    }

    FFrameDeltaTime = std::min(CurrentGameplayTime() - FFrameStartTime, FFrameDurationTimer.ElapsedTime<ETimePeriod::SECONDS>());
    FFrameStartTime += FFrameDeltaTime;
    FFrameDurationTimer.Start();
    ++FFrameNumber;
}

void TimeManagerImpl::NewGameplayTick()
{
    FCurrentGameplayTick++;
}

void TimeManagerImpl::End()
{
    FGlobalTimer.Stop();
}

namespace TimeManager
{

void Create()
{
    TimeManagerImpl::CreateIFP();
}

void NewFrame()
{
    TimeManagerImpl::Instance().NewFrame();
}

void NewGameplayTick()
{
    LOG_GAMEPLAY(fmt::format("Current gameplay tick {}", TimeManagerImpl::Instance().CurrentGameplayTick()));
    TimeManagerImpl::Instance().NewGameplayTick();
}

void End()
{
    TimeManagerImpl::Instance().End();
    TimeManagerImpl::Destroy();
}

const float GameplayDeltaTime()
{
    return GameplayTickSize;
}

const float CurrentGameplayTime()
{
    return GameplayCurrentTick() * GameplayDeltaTime();
}

const u32 GameplayCurrentTick()
{
    return TimeManagerImpl::Instance().CurrentGameplayTick();
}

const float FrameDeltaTime()
{
    return TimeManagerImpl::Instance().FrameDeltaTime();
}

const float FrameStartTime()
{
    return TimeManagerImpl::Instance().FrameStartTime();
}

const float DurationSinceStartRealTime()
{
    return TimeManagerImpl::Instance().DurationSinceStartRealTime();
}

const u32 GetFrameNumber()
{
    return TimeManagerImpl::Instance().GetFrameNumber();
}

} // namespace TimeManager

} // namespace ECSEngine
