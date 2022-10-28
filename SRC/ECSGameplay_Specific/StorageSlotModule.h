
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
    
    using StartingResources = std::pair<GameResource::Type, u32>;
public:
    StorageSlotModuleTemplate()
        : ModuleTemplate()
    {
    }
    ~StorageSlotModuleTemplate() { }

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    MemoryView<const StartingResources> GetStartingResources() const { return MemoryView<const StartingResources>(FStartingResources.data(), (u32)FStartingResources.size()); }

    SERIALIZE()
    {
        PROPERTYFIELD(HasInitialResources, false);
        PROPERTYFIELD(NumberOfSlots, 0);
        PROPERTYFIELD(SlotSize, 0);
        PROPERTYFIELD(RadiusOfEffect, 0.f);
        PROPERTYFIELD(StartingResources, std::vector<StartingResources>());
    }

    bool HasInitialResources() const { return FHasInitialResources; }
    u32 NumberOfSlots() const { return FNumberOfSlots; }
    u32 SlotSize() const { return FSlotSize; }
    float RadiusOfEffect() const { return FRadiusOfEffect; }

protected:
    void VirtualDrawEditor() override;

private:
    bool FHasInitialResources = false;
    u32 FNumberOfSlots = 0;
    u32 FSlotSize = 0;
    float FRadiusOfEffect = 0.f;
    std::vector<StartingResources> FStartingResources;
};

struct StorageSlot
{
    DECLARE_SAVELOAD_ABILITIES();

public:
    GameResource::Type Resource = GameResource::LENGTH;
    u32 Quantity = 0;
    EntityId FReservedForBuilding;
};

class StorageSlotModule : public Module
{
    DECLARE_MODULE(StorageSlotModule);

    DECLARE_SAVELOAD_ABILITIES();

public:
    StorageSlotModule();
    virtual ~StorageSlotModule();

    u32 NumberOfSlots() const;
    u32 SlotSize() const;
    float RadiusOfEffect() const;
    i32 FreeSlots() const;

    bool ReserveSlotsIFP(const EntityId& parUnitId, MemoryView<const GameResource::Type> parRessources);
    void RemoveSlotsReservationsIFN(const EntityId& parUnitId);

    u32 GetNbResources(const GameResource::Type parResource) const;
    u32 GetFreeSpaceInSlot(const EntityId& parUnitId, const GameResource::Type parResource) const;
    u32 AddResourceInSlot(const EntityId& parUnitId, const GameResource::Type parResource, const u32 parQuantity);
    u32 RemoveResourceInSlot(const GameResource::Type parResource, const u32 parQuantity);

    MemoryView<const StorageSlot> StorageSlots() const { return MemoryView(FSlots.data(), FSlots.size()); }

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;

protected:
    std::vector<StorageSlot> FSlots;
    i32 FFreeSlots = 0;
    u32 FSlotSize = 0;
    float FRadiusOfEffect = 0.f;
};

} // namespace ECSEngine
