
#pragma once
#include "Common/MemoryView.h"
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"
#include "GameResources.h"

namespace ECSEngine
{
class StorageSlotModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(StorageSlotModule, StorageSlotModuleTemplate);

public:
    StorageSlotModuleTemplate()
        : ModuleTemplate()
    {
    }
    ~StorageSlotModuleTemplate() { }

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    SERIALIZE()
    {
        PROPERTYFIELD(NumberOfSlots, 0);
        PROPERTYFIELD(SlotSize, 0);
        PROPERTYFIELD(RadiusOfEffect, 0.f);
    }

    u32 NumberOfSlots() const { return FNumberOfSlots; }
    u32 SlotSize() const { return FSlotSize; }
    float RadiusOfEffect() const { return FRadiusOfEffect; }

protected:
    void VirtualDrawEditor() override;

private:
    u32 FNumberOfSlots = 0;
    u32 FSlotSize = 0;
    float FRadiusOfEffect = 0.f;
};

struct StorageSlot
{
    GameResource::Type Resource = GameResource::LENGTH;
    u32 Quantity = 0;
    EntityId FReservedForBuilding;
};

class StorageSlotModule : public Module
{
    DECLARE_MODULE(StorageSlotModule);

public:
    StorageSlotModule();
    ~StorageSlotModule();

    u32 NumberOfSlots() const;
    u32 SlotSize() const;
    float RadiusOfEffect() const;
    i32 FreeSlots() const;

    bool ReserveSlotsIFP(const EntityId& parUnitId, MemoryView<const GameResource::Type> parRessources);

    u32 GetFreeSpaceInSlot(const EntityId& parUnitId, const GameResource::Type parResource) const;
    u32 AddResourceInSlot(const EntityId& parUnitId, const GameResource::Type parResource, const u32 parQuantity);
    u32 RemoveResourceInSlot(const EntityId& parUnitId, const GameResource::Type parResource, const u32 parQuantity);

    MemoryView<const StorageSlot> StorageSlots() const { return MemoryView(FSlots.data(), FSlots.size()); }

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;

private:
    std::vector<StorageSlot> FSlots;
    i32 FFreeSlots = 0;
};

} // namespace ECSEngine
