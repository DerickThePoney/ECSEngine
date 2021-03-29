#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"
#include "HarvesterState.h"

namespace ECSEngine
{
class ResourceHarvesterModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(ResourceHarvesterModule, ResourceHarvesterModuleTemplate);

public:
    ResourceHarvesterModuleTemplate()
        : ModuleTemplate()
    {
    }

    virtual ~ResourceHarvesterModuleTemplate() { }

    virtual Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    float HarvestingDuration() const { return FHarvestingDuration; }

    SERIALIZE() { PROPERTYFIELD(HarvestingDuration, 0.0f); }

protected:
    virtual void VirtualDrawEditor() override;

private:
    float FHarvestingDuration = 0.f;
};

class ResourceHarvesterModule final : public Module
{
    DECLARE_MODULE(ResourceHarvesterModule);

public:
    ResourceHarvesterModule()
        : Module()
    {
    }

    void SetHarvesterState(HarvesterState::Type parState) { FHarvesterState = parState; }
    void SetTarget(const EntityId& parTarget) { FTarget = parTarget; }
    void SetRemainingHarvestingDuration(const float parDuration) { FRemainingHarvestingDuration = parDuration; }

    HarvesterState::Type HarvesterState() const { return FHarvesterState; }
    const EntityId& Target() const { return FTarget; }
    float RemainingHarvestingDuration() const { return FRemainingHarvestingDuration; }

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parContainer);

private:
    EntityId FTarget;

    HarvesterState::Type FHarvesterState = HarvesterState::IDLE;
    float FRemainingHarvestingDuration = 0.0f;
};
} // namespace ECSEngine
