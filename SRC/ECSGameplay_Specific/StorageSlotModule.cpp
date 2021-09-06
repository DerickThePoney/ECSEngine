
#include "stdafx.h"

#include "StorageSlotModule.h"

#include "Application/PropertyDrawer.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::StorageSlotModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::StorageSlotModuleTemplate);

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(StorageSlotModule, StorageSlotModuleTemplate);

Module* StorageSlotModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<StorageSlotModule>(this, parUnitId, parParameters);
}

void StorageSlotModuleTemplate::VirtualDrawEditor()
{
    EDITOR_PROPERTY_SIMPLE("Number of slots", FNumberOfSlots);
    EDITOR_PROPERTY_SIMPLE("Slot size", FSlotSize);
}

StorageSlotModule::StorageSlotModule()
    : Module()
{
}

StorageSlotModule::~StorageSlotModule()
{
}

u32 StorageSlotModule::NumberOfSlots() const
{
    const StorageSlotModuleTemplate* t = Template<StorageSlotModuleTemplate>();
    AssertRelease(t != nullptr);
    return t->NumberOfSlots();
}

u32 StorageSlotModule::SlotSize() const
{
    const StorageSlotModuleTemplate* t = Template<StorageSlotModuleTemplate>();
    AssertRelease(t != nullptr);
    return t->SlotSize();
}

float StorageSlotModule::RadiusOfEffect() const
{
    const StorageSlotModuleTemplate* t = Template<StorageSlotModuleTemplate>();
    AssertRelease(t != nullptr);
    return t->RadiusOfEffect();
}

bool StorageSlotModule::ReserveSlotsIFP(const EntityId& parUnitId, MemoryView<const GameResource::Type> parRessources)
{
    AlwaysCheckedAssert(parRessources.size() < NumberOfSlots());
    if (parRessources.size() >= NumberOfSlots())
        return false;

    std::vector<u32> possibleSlotsIndices;
    possibleSlotsIndices.reserve(parRessources.size());

    forrange(i, 0, FSlots.size())
    {
        const StorageSlot& slot = FSlots[i];
        if (slot.FReservedForBuilding.Valid())
            continue;

        possibleSlotsIndices.push_back(i);

        if (possibleSlotsIndices.size() == parRessources.size())
            break;
    }

    AlwaysCheckedAssert(possibleSlotsIndices.size() <= parRessources.size());
    if (possibleSlotsIndices.size() != parRessources.size())
        return false;

    u32 resIdx = 0;
    foreachitemconst(idx, possibleSlotsIndices)
    {
        StorageSlot& slot = FSlots[idx];
        slot.FReservedForBuilding = parUnitId;
        slot.Quantity = 0;
        slot.Resource = parRessources[resIdx++];
    }
    return true;
}

void StorageSlotModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);

    const u32 nbSlots = NumberOfSlots();
    FSlots.resize(nbSlots);
}

} // namespace ECSEngine
