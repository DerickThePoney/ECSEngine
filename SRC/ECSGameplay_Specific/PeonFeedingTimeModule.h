#pragma once

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

    float InitialLifeSpan() const { return FInitialLifeSpan; }

    SERIALIZE() { PROPERTYFIELD(InitialLifeSpan, 90.f); }

protected:
    void VirtualDrawEditor() override;

private:
    float FInitialLifeSpan = 90.f;
};

class PeonFeedingTimeModule final : public Module
{
    DECLARE_MODULE(PeonFeedingTimeModule);

public:
    PeonFeedingTimeModule();
    ~PeonFeedingTimeModule() { }

    float RemainingLifeSpan() const { return FRemainingLifeSpan; }
    void SetRemainingLifeSpan(float parLifeTime) { FRemainingLifeSpan = parLifeTime; }

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;

private:
    float FRemainingLifeSpan = 0.f;
};
} // namespace ECSEngine
