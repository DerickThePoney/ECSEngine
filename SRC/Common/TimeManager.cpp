#include "stdafx.h"

#include "TimeManager.h"

namespace ECSEngine
{

TimeManager::TimeManager()
    : Singleton()
    , FFrameDeltaTime(0.0f)
    , FDurationSinceStartBeginingOfFrame(0.0f)
    , FFrameNumber(0)
{
}

TimeManager::~TimeManager()
{
}

void TimeManager::Start()
{
    FGlobalTimer.Start();
}

void TimeManager::NewFrame()
{
    if (FFrameNumber != 0)
    {
        FFrameDurationTimer.Stop();
    }

    FFrameDeltaTime = FFrameDurationTimer.ElapsedTime();
    FDurationSinceStartBeginingOfFrame = FGlobalTimer.PeekDurationSinceStart();
    FFrameDurationTimer.Start();
    ++FFrameNumber;
}

void TimeManager::End()
{
    FGlobalTimer.Stop();
}

} // namespace ECSEngine
