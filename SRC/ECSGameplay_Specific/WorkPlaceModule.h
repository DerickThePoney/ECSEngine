
#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{
class WorkPlaceModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(WorkPlaceModule, WorkPlaceModuleTemplate);

public:
    WorkPlaceModuleTemplate()
        : ModuleTemplate()
    {
    }
    ~WorkPlaceModuleTemplate() { }

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    SERIALIZE() { PROPERTYFIELD(MaxWorkers, 0); }

    const u32 MaxWorkers() const { return FMaxWorkers; }

protected:
    void VirtualDrawEditor() override;

private:
    u32 FMaxWorkers;
};

class WorkPlaceModule : public Module
{
    DECLARE_MODULE(WorkPlaceModule);

public:
    WorkPlaceModule();
    ~WorkPlaceModule();

    u32 MaxWorkers() const { return Template<WorkPlaceModuleTemplate>()->MaxWorkers(); }
    u32 RemainingJobs() const;

    void AddNewWorker(const EntityId& parUnitId);
    void FireWorker(const EntityId& parUnitId);

    bool IsWorkingHere(const EntityId& parUnitId) const;

private:
    std::vector<EntityId> FAssignedWorkers;
};

} // namespace ECSEngine
