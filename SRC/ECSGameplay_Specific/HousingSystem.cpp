#include "stdafx.h"

#include "HousingSystem.h"

#include "ECSCore/ModuleAccessor.h"
#include "HousingPlaceModule.h"
#include "LinkToHousingPlaceModule.h"
#include "ECSGameplay_Common/WorldIds.h"

namespace ECSEngine
{

HousingSystem::HousingSystem()
    : ModuleSystem()
{
    RegisterDepency<LinkToHousingPlaceModule>(EEntityWorlds::PEONS);
    RegisterDepency<HousingPlaceModule>(EEntityWorlds::BUILDINGS);
}

void HousingSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<HousingPlaceModule> housingPlaceController(EEntityWorlds::BUILDINGS);

    // 1- loop though the places to look for free spots
    foreachitemconst(housingPlace, housingPlaceController)
    {
        auto itFind = FFreeHousingSpots.find(housingPlace.UnitId());
        const u32 remainingFreeSpots = housingPlace.RemainingFreeSpace();
        if (itFind == FFreeHousingSpots.end())
        {
            if (remainingFreeSpots > 0)
                FFreeHousingSpots.insert_or_assign(housingPlace.UnitId(), remainingFreeSpots);
        }
        else
        {
            if (remainingFreeSpots == 0)
            {
                FFreeHousingSpots.erase(itFind);
            }
            else
            {
                itFind->second = remainingFreeSpots;
            }
        }
    }

    // early bail...
    if (FFreeHousingSpots.empty())
        return;

    // 2- Loop through the peons to check for the ones that don't have any house
    ModuleAccessor<LinkToHousingPlaceModule> linkToHousingPlaceController(EEntityWorlds::PEONS);
    FHomelessPeons.clear();
    foreachitemconst(linkToHouse, linkToHousingPlaceController)
    {
        if (!linkToHouse.HouseId().Valid())
        {
            FHomelessPeons.push_back(linkToHouse.UnitId());
        }
    }

    // 3- Assign peons to house
    auto freeSpot = FFreeHousingSpots.begin();
    while (!FHomelessPeons.empty())
    {
        const EntityId& homeless = FHomelessPeons.front();
        AlwaysCheckedAssert(freeSpot->second > 0);
        freeSpot->second -= 1;

        HousingPlaceModule* house = housingPlaceController[freeSpot->first];
        AssertRelease(house != nullptr);
        AlwaysCheckedAssert(!house->IsResidentInHere(homeless));
        house->AddNewResident(homeless);
        AlwaysCheckedAssert(house->IsResidentInHere(homeless));

        LinkToHousingPlaceModule* lthouse = linkToHousingPlaceController[homeless];
        lthouse->SetHouseId(freeSpot->first);

        FHomelessPeons.pop_front();

        if (freeSpot->second == 0)
        {
            freeSpot++;
            if (freeSpot == FFreeHousingSpots.end())
            {
                break;
            }
        }
    }
}

} // namespace ECSEngine
