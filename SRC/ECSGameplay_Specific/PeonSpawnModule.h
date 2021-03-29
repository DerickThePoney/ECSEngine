#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"
#include "GameResources.h"
#include "GameplayActions.h"

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

    SERIALIZE() { }

protected:
    void VirtualDrawEditor() override;
};

class PeonSpawnModule : public Module
{
    DECLARE_MODULE(PeonSpawnModule)
public:
    PeonSpawnModule();
    ~PeonSpawnModule();

    bool RequestedPeonCreation() const { return FRequestedPeonCreation; }
    void RequestPeonSapwn(SpawnPeonOrder&& parSpawnOrder)
    {
        FSpawnOrder = std::move(parSpawnOrder);
        FRequestedPeonCreation = true;
    }

    SpawnPeonOrder PopPeonSpawnOrder()
    {
        AssertRelease(FRequestedPeonCreation);
        FRequestedPeonCreation = false;
        return FSpawnOrder;
    }

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parContainer) override;

private:
    SpawnPeonOrder FSpawnOrder;
    bool FRequestedPeonCreation = false;
};
} // namespace ECSEngine
