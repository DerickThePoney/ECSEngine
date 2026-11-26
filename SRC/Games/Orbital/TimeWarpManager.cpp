#include "stdafx.h"

#include "TimeWarpManager.h"

#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"
#include "ECSCore/ScopedModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/RigidbodyModule.h"
#include "Physics/PhysicsAPI.h"
#include "PlanetAtmosphereModule.h"
#include "PlanetGravityModule.h"
#include "RocketManagerModule.h"

#include <fstream>

namespace ECSEngine
{
void FillDefault(std::vector<TimeWarpEntry>& WarpConfig)
{
    WarpConfig.clear();
    WarpConfig.resize(7);

    WarpConfig[1].FMinAltitudeRatio = 0.05f;
    WarpConfig[1].FMultiplier = 5;
    WarpConfig[2].FMinAltitudeRatio = 0.2f;
    WarpConfig[2].FMultiplier = 10;
    WarpConfig[3].FMinAltitudeRatio = 0.5f;
    WarpConfig[3].FMultiplier = 50;
    WarpConfig[4].FMinAltitudeRatio = 1.f;
    WarpConfig[4].FMultiplier = 100;
    WarpConfig[5].FMinAltitudeRatio = 2.f;
    WarpConfig[5].FMultiplier = 1000;
    WarpConfig[6].FMinAltitudeRatio = 5.f;
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

        const std::string fullFilename = GlobalResourceCache::Instance().FCache->GetBasePath() + "\\" + Filename;
        std::ofstream ofstr(fullFilename);
        cereal::JSONOutputArchive archive(ofstr);
        archive(NAMEDPROPERTY("TimeWarpConfig", FTimeWarpConfig));
    }
}

u32 TimeWarpManager::CurrentTimeWarpMultiplier() const
{
    AssertRelease(FCurrentTimeWarpIndex >= 0 && FCurrentTimeWarpIndex < FTimeWarpConfig.size());
    return FTimeWarpConfig[FCurrentTimeWarpIndex].FMultiplier;
}

float TimeWarpManager::CurrentTimeWarpAltitudeCapRatio() const
{
    AssertRelease(FCurrentTimeWarpIndex >= 0 && FCurrentTimeWarpIndex < FTimeWarpConfig.size());
    return FTimeWarpConfig[FCurrentTimeWarpIndex].FMinAltitudeRatio;
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

using AccessHelpers = ScopedModuleAccessor<MC<RocketManagerModule, EEntityWorlds::STANDARD>,
      MC<RigidbodyModule, EEntityWorlds::STANDARD>,
      MC<PlanetGravityModule, EEntityWorlds::STANDARD>,
      MC<PlanetAtmosphereModule, EEntityWorlds::STANDARD>>;

void TimeWarpManager::ClampToAltitude()
{
    AccessHelpers accessHelpers;

    AssertRelease(accessHelpers.Accessor<RocketManagerModule>().size() <= 1);
    if (accessHelpers.Accessor<RocketManagerModule>().size() == 0)
        return;

    if (CurrentTimeWarpMultiplier() == 1)
        return;

    RocketManagerModule& rocket = *accessHelpers.Accessor<RocketManagerModule>().begin();
    if (!rocket.CurrentGravityTarget().Valid())
        return;

    vec3 comPosition;
    rocket.ComputeActiveCenterOfMass(accessHelpers.Accessor<RigidbodyModule>(), comPosition);

    const PlanetGravityModule* const planetGravity = accessHelpers.GetModule<PlanetGravityModule>(rocket.CurrentGravityTarget());
    AssertRelease(planetGravity != nullptr);

    const RigidbodyModule* const planetBody = accessHelpers.GetModule<RigidbodyModule>(rocket.CurrentGravityTarget());
    AssertRelease(planetBody != nullptr);

    vec3 planetComPos;
    if (!Physics::GetBodyCenterOfMass(planetBody->BodyHandle(), planetComPos))
    {
        return;
    }

    float AtmosphereHeight = 0.f;
    const PlanetAtmosphereModule* const planetAtmosphere = accessHelpers.GetModule<PlanetAtmosphereModule>(rocket.CurrentGravityTarget());
    if (planetAtmosphere != nullptr)
    {
        AtmosphereHeight = planetAtmosphere->AtmosphereHeight();
    }

    const float Altitude = Length(comPosition - planetComPos) - planetGravity->PlanetRadius();

    while (CurrentTimeWarpIndex() > 0)
    {
        const float AltCap = planetGravity->PlanetRadius() * CurrentTimeWarpAltitudeCapRatio() + AtmosphereHeight;
        if (Altitude >= AltCap)
            break;
        DecrementTimeWarpIndex();
    }
}

} // namespace ECSEngine
