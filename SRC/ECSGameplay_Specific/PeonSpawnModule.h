#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"
#include "GameResources.h"

namespace ECSEngine
{
class EntityTemplate;
class PeonSpawnModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(PeonSpawnModule, PeonSpawnModuleTemplate);

public:
    PeonSpawnModuleTemplate()
        : ModuleTemplate()
    {
    }
    ~PeonSpawnModuleTemplate() { }

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    SERIALIZE()
    {
        PROPERTYFIELD(PeonTemplateName, "Default peon template");
        PROPERTYFIELD(BaseCost, 10);
        PROPERTYFIELD(Multiplier, 1.05f);
        PROPERTYFIELD(ResourceToPay, GameResource::FOOD);
    }

    const EntityTemplate* PeonTemplate() const { return FPeonTemplate; }
    u32 CostForNextSpawn(const u32 parCurrentPeonsQuantity) const;
    GameResource::Type ResourceToPay() const { return FResourceToPay; }

protected:
    void VirtualDrawEditor() override;
    void VirtualPostLoad() override;

private:
    void SetUpTemplate();

private:
    std::string FPeonTemplateName = "Default peon template";
    const EntityTemplate* FPeonTemplate = nullptr;

    u32 FBaseCost = 10;
    float FMultiplier = 1.05f;
    GameResource::Type FResourceToPay = GameResource::FOOD;
};

class PeonSpawnModule : public Module
{
    DECLARE_MODULE(PeonSpawnModule)
public:
    PeonSpawnModule();
    ~PeonSpawnModule();

    u32 CostForNextSpawn(const u32 parCurrentPeonsQuantity) const;
    GameResource::Type ResourceToPay() const;

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parContainer) override;
};
} // namespace ECSEngine