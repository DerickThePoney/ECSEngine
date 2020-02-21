#pragma once
#include "Singleton.h"
#include "Timer.h"

namespace ECSEngine
{
class TimeManager : public Singleton<TimeManager>
{
public:
    TimeManager();
    ~TimeManager();

    void Start();
    void NewFrame();
    void End();

    const float FrameDeltaTime() const { return FFrameDeltaTime; }
    const float DurationSinceStartBeginingOfFrame() const { return FDurationSinceStartBeginingOfFrame; }
    const float DurationSinceStartRealTime() const { return FGlobalTimer.PeekDurationSinceStart<ETimePeriod::SECONDS>(); }
    const u32 GetFrameNumber() const { return FFrameNumber; }

private:
    Timer FGlobalTimer;
    Timer FFrameDurationTimer;

    float FDurationSinceStartBeginingOfFrame;
    float FFrameDeltaTime;
    u32 FFrameNumber;
};
} // namespace ECSEngine