#include "stdafx.h"

#include "GFXRepresentationManager.h"

#include "GFXSelectable.h"
#include "SkelettonManager.h"

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

    SkelettonManager::Instance().UpdateSkinningMatrices();
}

void GFXRepresentationManager::OnGameplayFrameEnded()
{
    FGameplayFrameEnded = true;
    // swap the queues here? transfer queue from proxy to representations?
}

std::pair<bool, bool> GFXRepresentationManager::IsGFXSelectedOrHighlighted(const u32 parId) const
{
    std::scoped_lock<std::mutex> lock(FMutex);
    AssertRelease(parId != -1);
    auto itFind = FGFXRepresentations.find(parId);
    AssertRelease(itFind != FGFXRepresentations.end());
    AssertRelease(itFind->second != nullptr);

    const GFXRepresentation* rep = itFind->second.get();

    const bool selected = rep->GetSelectable() != nullptr && rep->GetSelectable()->IsSelected();
    const bool highlithed = rep->GetSelectable() != nullptr && rep->GetSelectable()->IsHighlighted();

    return { selected, highlithed };
}

u32 GFXRepresentationManager::CreateGFXRepresentation(const GFXRepresentationInitialiser& parInit)
{
    std::scoped_lock<std::mutex> lock(FMutex);
    const u32 newId = FGFXIdGenerator.GetNextId();
    GFXRepresentation* newRep = new GFXRepresentation(newId);
    newRep->Initialise(parInit);
    FGFXRepresentations.insert_or_assign(newId, std::unique_ptr<GFXRepresentation>(newRep));
    return newId;
}

void GFXRepresentationManager::DeleteGFXRepresentation(const u32 parId)
{
    std::scoped_lock<std::mutex> lock(FMutex);
    AssertRelease(parId != -1);
    auto itFind = FGFXRepresentations.find(parId);
    AssertRelease(itFind != FGFXRepresentations.end());
    AssertRelease(itFind->second != nullptr);
    FGFXRepresentations.erase(itFind);
    FGFXIdGenerator.ReleaseId(parId);
}

} // namespace Rendering
} // namespace ECSEngine
