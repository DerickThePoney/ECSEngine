#include "stdafx.h"

#include "TimeManager.h"

#include "Singleton.h"
#include "Timer.h"

namespace ECSEngine
{

class TimeManagerImpl : public Singleton<TimeManagerImpl>
{
public:
    TimeManagerImpl();
    ~TimeManagerImpl();

    void Start();
    void NewFrame();
    void End();

    const float FrameDeltaTime() const { return FFrameDeltaTime; }
    const float FrameStartTime() const { return FFrameStartTime; }
    const float DurationSinceStartRealTime() const { return FGlobalTimer.PeekDurationSinceStart<ETimePeriod::SECONDS>(); }
    const u32 GetFrameNumber() const { return FFrameNumber; }

private:
    Timer FGlobalTimer;
    Timer FFrameDurationTimer;

    float FFrameStartTime;
    float FFrameDeltaTime;
    u32 FFrameNumber;
};

TimeManagerImpl::TimeManagerImpl()
    : Singleton()
    , FFrameDeltaTime(0.0f)
    , FFrameNumber(0)
{
}

TimeManagerImpl::~TimeManagerImpl()
{
}

void TimeManagerImpl::Start()
{
    FGlobalTimer.Start();
}

void TimeManagerImpl::NewFrame()
{
    if (FFrameNumber != 0)
    {
        FFrameDurationTimer.Stop();
    }

    FFrameStartTime = DurationSinceStartRealTime();
    FFrameDeltaTime = FFrameDurationTimer.ElapsedTime<ETimePeriod::SECONDS>();
    FFrameDurationTimer.Start();
    ++FFrameNumber;
}

void TimeManagerImpl::End()
{
    FGlobalTimer.Stop();
}

namespace TimeManager
{

void Start()
{
    TimeManagerImpl::CreateIFP();
    TimeManagerImpl::Instance().Start();
}

void NewFrame()
{
    TimeManagerImpl::Instance().NewFrame();
}

void End()
{
    TimeManagerImpl::Instance().End();
    TimeManagerImpl::Destroy();
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
