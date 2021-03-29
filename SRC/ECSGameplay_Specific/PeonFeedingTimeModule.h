#pragma once

#include "Common/ModifiableValue.h"
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{
namespace ModuleParameters
{
class ParameterContainer;
}

class EntityId;

class PeonFeedingTimeModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(PeonFeedingTimeModule, PeonFeedingTimeModuleTemplate);

public:
    PeonFeedingTimeModuleTemplate()
        : ModuleTemplate()
    {
    }

    virtual Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    float TimeBetweenTwoFeedTime() const { return FTimeBetweenTwoFeedTime; }

    SERIALIZE() { PROPERTYFIELD(TimeBetweenTwoFeedTime, 90.f); }

protected:
    void VirtualDrawEditor() override;

private:
    float FTimeBetweenTwoFeedTime = 90.f;
};

class PeonFeedingTimeModule final : public Module
{
    DECLARE_MODULE(PeonFeedingTimeModule);

public:
    PeonFeedingTimeModule();
    ~PeonFeedingTimeModule() { }

    float RemainTimeBeforeNextFeedAsRatio() const { return FRemainingTimeBetweenTwoFeedTimes / FTimeBetweenFeedingTimes.ComputedValue(); }
    float RemainingTimeBeforeNextFeed() const { return FRemainingTimeBetweenTwoFeedTimes; }
    void ResetFeedingTime() { FRemainingTimeBetweenTwoFeedTimes = FTimeBetweenFeedingTimes.ComputedValue(); }
    void SetRemainingTimeBeforeNextFeedingTime(float parLifeTime) { FRemainingTimeBetweenTwoFeedTimes = parLifeTime; }

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;

private:
    ModifiableValue<float> FTimeBetweenFeedingTimes;
    float FRemainingTimeBetweenTwoFeedTimes = 0.f;
};
} // namespace ECSEngine
