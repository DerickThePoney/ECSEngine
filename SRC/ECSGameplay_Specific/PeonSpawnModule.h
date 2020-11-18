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
        PROPERTYFIELD(AutoSpawn, false);
    }

    const EntityTemplate* PeonTemplate() const { return FPeonTemplate; }
    u32 CostForNextSpawn(const u32 parCurrentPeonsQuantity) const;
    GameResource::Type ResourceToPay() const { return FResourceToPay; }
    bool AutoSpawn() const { return FAutoSpawn; }

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
    bool FAutoSpawn = false;
};

class PeonSpawnModule : public Module
{
    DECLARE_MODULE(PeonSpawnModule)
public:
    PeonSpawnModule();
    ~PeonSpawnModule();

    const EntityTemplate* PeonTemplate() const;
    u32 CostForNextSpawn(const u32 parCurrentPeonsQuantity) const;
    GameResource::Type ResourceToPay() const;
    bool AutoSpawn() const;

    bool RequestedPawnCreation() const { return FRequestedPawnCreation; }
    void SetRequestedPawnCreation(bool parValue) { FRequestedPawnCreation = parValue; }

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parContainer) override;

private:
    bool FRequestedPawnCreation = false;
};
} // namespace ECSEngine