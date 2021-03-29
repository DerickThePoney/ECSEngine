#include "stdafx.h"

#include "GFXRepresentationManager.h"

namespace ECSEngine
{
namespace Rendering
{

void GFXRepresentationManager::Update(float parCurrentTime)
{
    if (FGameplayFrameEnded)
    {
        std::scoped_lock<std::mutex> lock(FMutex);
        foreachitem(rep, FGFXRepresentations)
        {
            AssertRelease(rep.second != nullptr);
            rep.second->SwapQueues();
        }

        FGameplayFrameEnded = false;
    }

    foreachitem(rep, FGFXRepresentations)
    {
        AssertRelease(rep.second != nullptr);
        rep.second->Update(parCurrentTime);
    }
}

void GFXRepresentationManager::OnGameplayFrameEnded()
{
    FGameplayFrameEnded = true;
    // swap the queues here? transfer queue from proxy to representations?
}

u32 GFXRepresentationManager::CreateGFXRepresentation(const GFXRepresentationInitialiser& parInit)
{
    GFXRepresentation* newRep = new GFXRepresentation();
    newRep->Initialise(parInit);
    const u32 newId = FGFXIdGenerator.GetNextId();
    FGFXRepresentations.insert_or_assign(newId, std::unique_ptr<GFXRepresentation>(newRep));
    return newId;
}

void GFXRepresentationManager::DeleteGFXRepresentation(const u32 parId)
{
    AssertRelease(parId != -1);
    auto itFind = FGFXRepresentations.find(parId);
    AssertRelease(itFind != FGFXRepresentations.end());
    AssertRelease(itFind->second != nullptr);
    FGFXRepresentations.erase(itFind);
    FGFXIdGenerator.ReleaseId(parId);
}

} // namespace Rendering
} // namespace ECSEngine
