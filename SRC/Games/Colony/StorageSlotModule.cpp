
#include "stdafx.h"

#include "StorageSlotModule.h"

#include "Application/PropertyDrawer.h"
#include "Common/SavingSystemImplementation.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleUtils.h"
#include "ECSGameplaySpecificPropertyDrawers.h"

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
    EDITOR_PROPERTY_SIMPLE("Radius of effect", FRadiusOfEffect);

    EDITOR_PROPERTY_BOOL("Has initial resources ?", FHasInitialResources);
    if (FHasInitialResources)
    {
        auto drawStartingResource = [](std::string& parName, StartingResources& parValue)
        {
            ImGui::PushID(ImGui::GetID(&parValue));
            EDITOR_PROPERTY_GAME_RESOURCES("Resource", parValue.first, false);
            ImGui::SameLine();
            EDITOR_PROPERTY_SIMPLE("Qty", parValue.second);
            ImGui::PopID();
        };
        EDITOR_PROPERTY_COMPLEXVECTOR(StartingResources, "Starting resources", FStartingResources, false, drawStartingResource, true);

        if (FStartingResources.size() > FSlotSize)
        {
            FStartingResources.resize(FSlotSize);
        }
    }
}

IMPLEMENT_SAVELOAD_ABILITIES(StorageSlot);
template<typename Chunk, bool isWriting>
void StorageSlot::SaveLoad(Chunk& parChunk)
{
    parChunk& Resource;
    parChunk& Quantity;
    parChunk& FReservedForBuilding;
}

IMPLEMENT_SAVELOAD_ABILITIES(StorageSlotModule);
template<typename Chunk, bool isWriting>
void StorageSlotModule::SaveLoad(Chunk& parChunk)
{
    parent_type::SaveLoad(parChunk);
    parChunk& FSlots;
    parChunk& FFreeSlots;
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
    return FSlots.size();
}

u32 StorageSlotModule::SlotSize() const
{
    return FSlotSize;
}

float StorageSlotModule::RadiusOfEffect() const
{
    return FRadiusOfEffect;
}

i32 StorageSlotModule::FreeSlots() const
{
    return FFreeSlots;
}

// TODO: ReserveSlots for same resource with no building anymore ?
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
        FFreeSlots -= 1;
        AlwaysCheckedAssert(FFreeSlots >= 0);
    }
    return true;
}

void StorageSlotModule::RemoveSlotsReservationsIFN(const EntityId& parUnitId)
{
    foreachitem(slot, FSlots)
    {
        if (slot.FReservedForBuilding == parUnitId)
            slot.FReservedForBuilding = EntityId();
    }
}

u32 StorageSlotModule::GetNbResources(const GameResource::Type parResource) const
{
    u32 res = 0;
    foreachitem(slot, FSlots)
    {
        if (slot.Resource != parResource)
            continue;
        res += slot.Quantity;
    }
    return res;
}

u32 StorageSlotModule::GetFreeSpaceInSlot(const EntityId& parUnitId, const GameResource::Type parResource) const
{
    foreachitemconst(slot, FSlots)
    {
        if (slot.FReservedForBuilding != parUnitId)
            continue;
        if (slot.Resource != parResource)
            continue;
        return SlotSize() - slot.Quantity;
    }

    AssertNotReached();
    return 0;
}

u32 StorageSlotModule::AddResourceInSlot(const EntityId& parUnitId, const GameResource::Type parResource, const u32 parQuantity)
{
    foreachitem(slot, FSlots)
    {
        if (slot.FReservedForBuilding != parUnitId)
            continue;
        if (slot.Resource != parResource)
            continue;
        AlwaysCheckedAssert((SlotSize() - slot.Quantity) >= parQuantity);
        slot.Quantity += parQuantity;
        return parQuantity;
    }

    AssertNotReached();
    return 0;
}

u32 StorageSlotModule::RemoveResourceInSlot(const GameResource::Type parResource, const u32 parQuantity)
{
    u32 toRemove = parQuantity;
    u32 currentlyRemoved = 0;
    foreachitem(slot, FSlots)
    {
        if (slot.Resource != parResource)
            continue;
        const u32 resourceToRemove = Min(slot.Quantity, toRemove);
        slot.Quantity -= resourceToRemove;
        currentlyRemoved += resourceToRemove;
        toRemove -= resourceToRemove;

        if (slot.Quantity == 0 && !slot.FReservedForBuilding.Valid())
        {
            slot.Resource = GameResource::LENGTH;
            slot.FReservedForBuilding = EntityId();
            FFreeSlots += 1;
        }

        if (toRemove == 0)
            return parQuantity;
    }

    AssertNotReached();
    return currentlyRemoved;
}

void StorageSlotModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);

    const StorageSlotModuleTemplate* t = Template<StorageSlotModuleTemplate>();
    AssertRelease(t != nullptr);
    const u32 nbSlots = t->NumberOfSlots();
    FSlots.resize(nbSlots);
    FFreeSlots = (i32)nbSlots;
    FSlotSize = t->SlotSize();
    FRadiusOfEffect = t->RadiusOfEffect();

    if (t->HasInitialResources())
    {
        MemoryView<const StorageSlotModuleTemplate::StartingResources> startingRes = t->GetStartingResources();
        foreachitemconst(startRes, startingRes)
        {
            bool res = ReserveSlotsIFP(parUnitId, MakeConstViewOnSingleItem(startRes.first));
            AssertRelease(res);
            const u32 resourceAdded = AddResourceInSlot(parUnitId, startRes.first, startRes.second);
            AssertRelease(resourceAdded == startRes.second);
        }
    }
}

} // namespace ECSEngine
