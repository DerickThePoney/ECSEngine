#pragma once

#include "Common/Singleton.h"

namespace ECSEngine
{
struct TimeWarpEntry
{
    u32 FMultiplier = 1;
    float FMinAltitude = 0.f;

    SERIALIZE()
    {
        PROPERTYFIELD(Multiplier, 1);
        PROPERTYFIELD(MinAltitude, 0.f);
    }
};

class TimeWarpManager : public Singleton<TimeWarpManager>
{
public:
    void Initialize();

    u32 CurrentTimeWarpIndex() const { return FCurrentTimeWarpIndex; }
    u32 CurrentTimeWarpMultiplier() const;
    float CurrentTimeWarpAltitudeCap() const;

    void IncrementTimeWarpIndex();
    void DecrementTimeWarpIndex();

    void SetTimeWarpIndex(const u32 Value);

private:
    u32 FCurrentTimeWarpIndex = 0;

    std::vector<TimeWarpEntry> FTimeWarpConfig;
};
} // namespace ECSEngine