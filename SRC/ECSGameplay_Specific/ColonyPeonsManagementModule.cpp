#include "stdafx.h"

#include "ColonyPeonsManagementModule.h"

#include "ECSCore/EntityId.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::ColonyPeonsManagementModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::ColonyPeonsManagementModuleTemplate)

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(ColonyPeonsManagementModule, ColonyPeonsManagementModuleTemplate);

Module* ColonyPeonsManagementModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<ColonyPeonsManagementModule>(this, parUnitId, parParameters);
}

void ColonyPeonsManagementModuleTemplate::VirtualDrawEditor()
{
}

ColonyPeonsManagementModule::ColonyPeonsManagementModule()
    : Module()
{
}

void ColonyPeonsManagementModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);
}

void ColonyPeonsManagementModule::SetPeonOccupied(const EntityId& parPeon)
{
    AssertRelease(parPeon.GetWorldId() == Worlds::PEONS);
    auto it = std::find(FIdlePeons.begin(), FIdlePeons.end(), parPeon);
    AssertRelease(it != FIdlePeons.end());
    FIdlePeons.erase(it);

#ifdef PERFORM_SECURITY_CHECKS
    auto itOccupied = std::find(FOccupiedPeons.begin(), FOccupiedPeons.end(), parPeon);
    AssertRelease(itOccupied == FOccupiedPeons.end());
#endif
    FOccupiedPeons.push_back(parPeon);
}

void ColonyPeonsManagementModule::SetPeonIdleIFN(const EntityId& parPeon)
{
    AssertRelease(parPeon.GetWorldId() == Worlds::PEONS);
    auto it = std::find(FIdlePeons.begin(), FIdlePeons.end(), parPeon);
    if (it != FIdlePeons.end())
        return;

    auto itOccupied = std::find(FOccupiedPeons.begin(), FOccupiedPeons.end(), parPeon);
    AssertRelease(itOccupied != FOccupiedPeons.end());
    FOccupiedPeons.erase(itOccupied);
    FIdlePeons.push_back(parPeon);
}

void ColonyPeonsManagementModule::AddNewPeon(const EntityId& parPeon)
{
    AssertRelease(parPeon.GetWorldId() == Worlds::PEONS);
#ifdef PERFORM_SECURITY_CHECKS
    {
        auto it = std::find(FIdlePeons.begin(), FIdlePeons.end(), parPeon);
        AssertRelease(it == FIdlePeons.end());
    }
    {
        auto it = std::find(FOccupiedPeons.begin(), FOccupiedPeons.end(), parPeon);
        AssertRelease(it == FOccupiedPeons.end());
    }
#endif

    FIdlePeons.push_back(parPeon);
}

void ColonyPeonsManagementModule::RemovePeon(const EntityId& parPeon)
{
    AssertRelease(parPeon.GetWorldId() == Worlds::PEONS);

    {
        auto it = std::find(FIdlePeons.begin(), FIdlePeons.end(), parPeon);
        if (it != FIdlePeons.end())
        {
            FIdlePeons.erase(it);
            return;
        }
    }
    {
        auto it = std::find(FOccupiedPeons.begin(), FOccupiedPeons.end(), parPeon);
        if (it != FOccupiedPeons.end())
        {
            FOccupiedPeons.erase(it);
            return;
        }
    }

    AssertNotReached();
}

} // namespace ECSEngine
