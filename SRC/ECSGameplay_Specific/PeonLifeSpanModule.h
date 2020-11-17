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

class PeonLifeSpanModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(PeonLifeSpanModule, PeonLifeSpanModuleTemplate);

public:
    PeonLifeSpanModuleTemplate()
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

class PeonLifeSpanModule final : public Module
{
    DECLARE_MODULE(PeonLifeSpanModule);

public:
    PeonLifeSpanModule();
    ~PeonLifeSpanModule() { }

    float RemainingLifeSpan() const { return FRemainingLifeSpan; }
    void SetRemainingLifeSpan(float parLifeTime) { FRemainingLifeSpan = parLifeTime; }

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;

private:
    float FRemainingLifeSpan = 0.f;
};
} // namespace ECSEngine
