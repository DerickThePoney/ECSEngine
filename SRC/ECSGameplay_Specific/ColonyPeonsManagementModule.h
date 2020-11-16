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

class ColonyPeonsManagementModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(ColonyPeonsManagementModule, ColonyPeonsManagementModuleTemplate);

public:
    ColonyPeonsManagementModuleTemplate()
        : ModuleTemplate()
    {
    }

    virtual Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    SERIALIZE() { }

protected:
    void VirtualDrawEditor() override;
};

class ColonyPeonsManagementModule final : public Module
{
    DECLARE_MODULE(ColonyPeonsManagementModule);

public:
    ColonyPeonsManagementModule();
    ~ColonyPeonsManagementModule() { }

    const std::vector<EntityId>& IdlePeons() const { return FIdlePeons; }
    const std::vector<EntityId>& OccupiedPeons() const { return FOccupiedPeons; }
    std::vector<EntityId>& IdlePeons() { return FIdlePeons; }
    std::vector<EntityId>& OccupiedPeons() { return FOccupiedPeons; }

    void SetPeonOccupied(const EntityId& parPeon);
    void SetPeonIdleIFN(const EntityId& parPeon);
    void AddNewPeon(const EntityId& parPeon);

    u32 PeonsInColony() const { return (u32)(FOccupiedPeons.size() + FIdlePeons.size()); }

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;

private:
    std::vector<EntityId> FIdlePeons;
    std::vector<EntityId> FOccupiedPeons;
};
} // namespace ECSEngine