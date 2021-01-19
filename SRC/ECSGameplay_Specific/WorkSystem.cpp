#include "stdafx.h"

#include "WorkSystem.h"

#include "ECSCore/ModuleAccessor.h"
#include "LinkToWorkPlaceModule.h"
#include "WorkPlaceModule.h"

namespace ECSEngine
{

WorkSystem::WorkSystem()
    : ModuleSystem()
{
    RegisterDepency<LinkToWorkPlaceModule>(Worlds::PEONS);
    RegisterDepency<WorkPlaceModule>(Worlds::BUILDINGS);
}

void WorkSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<WorkPlaceModule> workPlaceController(Worlds::BUILDINGS);

    // 1- loop though the places to look for free spots
    foreachitemconst(workPlace, workPlaceController)
    {
        auto itFind = FFreeWorkJobs.find(workPlace.UnitId());
        const u32 remainingJobs = workPlace.RemainingJobs();
        if (itFind == FFreeWorkJobs.end())
        {
            if (remainingJobs > 0)
                FFreeWorkJobs.insert_or_assign(workPlace.UnitId(), remainingJobs);
        }
        else
        {
            if (remainingJobs == 0)
            {
                FFreeWorkJobs.erase(itFind);
            }
            else
            {
                itFind->second = remainingJobs;
            }
        }
    }

    // early bail...
    if (FFreeWorkJobs.empty())
        return;

    // 2- Loop through the peons to check for the ones that don't have any house
    ModuleAccessor<LinkToWorkPlaceModule> linkToWorkPlaceController(Worlds::PEONS);
    FJobLessPeons.clear();
    foreachitemconst(linkToJob, linkToWorkPlaceController)
    {
        if (!linkToJob.WorkPlaceId().Valid())
        {
            FJobLessPeons.push_back(linkToJob.UnitId());
        }
    }

    // 3- Assign peons to house
    auto freeSpot = FFreeWorkJobs.begin();
    while (!FJobLessPeons.empty())
    {
        const EntityId& jobless = FJobLessPeons.front();
        AlwaysCheckedAssert(freeSpot->second > 0);
        freeSpot->second -= 1;

        WorkPlaceModule* house = workPlaceController[freeSpot->first];
        AssertRelease(house != nullptr);
        AlwaysCheckedAssert(!house->IsWorkingHere(jobless));
        house->AddNewWorker(jobless);
        AlwaysCheckedAssert(house->IsWorkingHere(jobless));

        LinkToWorkPlaceModule* ltjob = linkToWorkPlaceController[jobless];
        ltjob->SetWorkPlaceId(freeSpot->first);

        FJobLessPeons.pop_front();

        if (freeSpot->second == 0)
        {
            freeSpot++;
            if (freeSpot == FFreeWorkJobs.end())
            {
                break;
            }
        }
    }
}

} // namespace ECSEngine
