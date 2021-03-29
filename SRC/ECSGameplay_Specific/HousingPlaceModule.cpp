
#include "stdafx.h"

#include "HousingPlaceModule.h"

#include "Application/PropertyDrawer.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::HousingPlaceModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::HousingPlaceModuleTemplate);

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(HousingPlaceModule, HousingPlaceModuleTemplate);

Module* HousingPlaceModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<HousingPlaceModule>(this, parUnitId, parParameters);
}

void HousingPlaceModuleTemplate::VirtualDrawEditor()
{
    EDITOR_PROPERTY_SIMPLE("Max place in building", FMaxPlace);
}

HousingPlaceModule::HousingPlaceModule()
    : Module()
{
}

HousingPlaceModule::~HousingPlaceModule()
{
}

void HousingPlaceModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);

    FAssociatedResidents.reserve(Template<HousingPlaceModuleTemplate>()->MaxPlace());
}

u32 HousingPlaceModule::RemainingFreeSpace() const
{
    return MaxPlace() - (u32)FAssociatedResidents.size();
}

void HousingPlaceModule::AddNewResident(const EntityId& parUnitId)
{
    AlwaysCheckedAssert(parUnitId.GetWorldId() == Worlds::PEONS);

    const u32 remainingSpace = RemainingFreeSpace();
    AlwaysCheckedAssert(remainingSpace > 0);
    if (remainingSpace == 0)
        return;

    const bool isAlreadyResident = IsResidentInHere(parUnitId);
    AlwaysCheckedAssert(!isAlreadyResident);
    if (isAlreadyResident)
        return;

    FAssociatedResidents.push_back(parUnitId);
}

void HousingPlaceModule::RemoveResident(const EntityId& parUnitId)
{
    AlwaysCheckedAssert(IsResidentInHere(parUnitId));

    forrange(i, 0, FAssociatedResidents.size())
    {
        if (FAssociatedResidents[i] == parUnitId)
        {
            FAssociatedResidents.erase(FAssociatedResidents.begin() + i);
            break;
        }
    }
}

bool HousingPlaceModule::IsResidentInHere(const EntityId& parUnitId) const
{
    foreachitemconst(resident, FAssociatedResidents)
    {
        if (resident == parUnitId)
            return true;
    }
    return false;
}

} // namespace ECSEngine
