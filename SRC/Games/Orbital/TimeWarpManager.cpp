#include "stdafx.h"

#include "TimeWarpManager.h"

#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"

#include <fstream>

namespace ECSEngine
{
void FillDefault(std::vector<TimeWarpEntry>& WarpConfig)
{
    WarpConfig.clear();
    WarpConfig.resize(7);

    WarpConfig[1].FMultiplier = 5;
    WarpConfig[2].FMultiplier = 10;
    WarpConfig[3].FMultiplier = 50;
    WarpConfig[4].FMultiplier = 100;
    WarpConfig[5].FMultiplier = 1000;
    WarpConfig[6].FMultiplier = 10000;
}

void TimeWarpManager::Initialize()
{
    const std::string Filename = "Configuration\\TimeWarpConfig.json";

    Resource r(Filename);
    if (GlobalResourceCache::Instance().FCache->FileExists(&r))
    {
        auto handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&r);
        AssertRelease(handle != nullptr);
        ResourceBuffer buff = handle->GetResourceBuffer();
        std::istream istr(&buff, std::istream::in);
        cereal::JSONInputArchive archive(istr);
        archive(NAMEDPROPERTY("TimeWarpConfig", FTimeWarpConfig));
    }
    else
    {
        FillDefault(FTimeWarpConfig);

        std::ofstream ofstr(GlobalResourceCache::Instance().FCache->GetBasePath() + Filename);
        cereal::JSONOutputArchive archive(ofstr);
        archive(NAMEDPROPERTY("TimeWarpConfig", FTimeWarpConfig));
    }
}

u32 TimeWarpManager::CurrentTimeWarpMultiplier() const
{
    AssertRelease(FCurrentTimeWarpIndex >= 0 && FCurrentTimeWarpIndex < FTimeWarpConfig.size());
    return FTimeWarpConfig[FCurrentTimeWarpIndex].FMultiplier;
}

float TimeWarpManager::CurrentTimeWarpAltitudeCap() const
{
    AssertRelease(FCurrentTimeWarpIndex >= 0 && FCurrentTimeWarpIndex < FTimeWarpConfig.size());
    return FTimeWarpConfig[FCurrentTimeWarpIndex].FMinAltitude;
}

void TimeWarpManager::IncrementTimeWarpIndex()
{
    if (FCurrentTimeWarpIndex < FTimeWarpConfig.size() - 1)
        FCurrentTimeWarpIndex++;
}

void TimeWarpManager::DecrementTimeWarpIndex()
{
    if (FCurrentTimeWarpIndex > 0)
        FCurrentTimeWarpIndex--;
}

void TimeWarpManager::SetTimeWarpIndex(const u32 Value)
{
    AssertRelease(Value >= 0 && Value < FTimeWarpConfig.size());
    FCurrentTimeWarpIndex = Value;
}

} // namespace ECSEngine
