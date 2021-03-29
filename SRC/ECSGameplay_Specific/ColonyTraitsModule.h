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
class ColonyTraitsModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(ColonyTraitsModule, ColonyTraitsModuleTemplate);

public:
    ColonyTraitsModuleTemplate()
        : ModuleTemplate()
    {
    }
    ~ColonyTraitsModuleTemplate() { }

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    SERIALIZE() { }

protected:
    void VirtualDrawEditor() override;
};

class ColonyTraitsModule final : public Module
{
    DECLARE_MODULE(ColonyTraitsModule);

public:
    ColonyTraitsModule()
        : Module()
    {
    }

    ~ColonyTraitsModule() { }

    float InfluenceRange() const { return FInfluenceRange.ComputedValue(); }
    void AddInfluenceModifier(const ValueModifier<float>& parModifer);
    void RemoveInfluenceModifier(const ValueModifier<float>& parModifier);

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parContainer) override;

private:
    ModifiableValue<float> FInfluenceRange;
};
} // namespace ECSEngine
