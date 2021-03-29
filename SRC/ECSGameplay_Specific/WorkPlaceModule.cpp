
#include "stdafx.h"

#include "WorkPlaceModule.h"

#include "Application/PropertyDrawer.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::WorkPlaceModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::WorkPlaceModuleTemplate);

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(WorkPlaceModule, WorkPlaceModuleTemplate);

Module* WorkPlaceModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<WorkPlaceModule>(this, parUnitId, parParameters);
}

void WorkPlaceModuleTemplate::VirtualDrawEditor()
{
    EDITOR_PROPERTY_SIMPLE("Maximum allowed workers", FMaxWorkers);
}

WorkPlaceModule::WorkPlaceModule()
    : Module()
{
}

WorkPlaceModule::~WorkPlaceModule()
{
}

u32 WorkPlaceModule::RemainingJobs() const
{
    return MaxWorkers() - (u32)FAssignedWorkers.size();
}

void WorkPlaceModule::AddNewWorker(const EntityId& parUnitId)
{
    AlwaysCheckedAssert(!IsWorkingHere(parUnitId));
    AlwaysCheckedAssert(RemainingJobs() > 0);

    if (RemainingJobs() == 0)
        return;

    FAssignedWorkers.push_back(parUnitId);
}

void WorkPlaceModule::FireWorker(const EntityId& parUnitId)
{
    AlwaysCheckedAssert(IsWorkingHere(parUnitId));

    forrange(i, 0, FAssignedWorkers.size())
    {
        if (FAssignedWorkers[i] == parUnitId)
        {
            FAssignedWorkers.erase(FAssignedWorkers.begin() + i);
            break;
        }
    }
}

bool WorkPlaceModule::IsWorkingHere(const EntityId& parUnitId) const
{
    foreachitemconst(worker, FAssignedWorkers)
    {
        if (worker == parUnitId)
            return true;
    }
    return false;
}

} // namespace ECSEngine
