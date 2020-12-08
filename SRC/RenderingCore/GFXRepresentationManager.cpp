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

void GFXRepresentationManager::CreateGFXRepresentation(const EntityId& parId, const GFXRepresentationInitialiser& parInit)
{
    auto itFind = FGFXRepresentations.find(parId);
    AlwaysCheckedAssert(itFind == FGFXRepresentations.end());
    if (itFind != FGFXRepresentations.end())
        return;

    GFXRepresentation* newRep = new GFXRepresentation();
    newRep->Initialise(parInit);
    FGFXRepresentations.insert_or_assign(parId, std::unique_ptr<GFXRepresentation>(newRep));
}

void GFXRepresentationManager::DeleteGFXRepresentation(const EntityId& parId)
{
    auto itFind = FGFXRepresentations.find(parId);
    if (itFind == FGFXRepresentations.end())
        return;

    FGFXRepresentations.erase(itFind);
}

} // namespace Rendering
} // namespace ECSEngine