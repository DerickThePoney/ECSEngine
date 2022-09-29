#include "stdafx.h"

#include "GFXRepresentationManager.h"

#include "GFXRepresentation.h"
#include "GFXSelectable.h"
#include "SkelettonManager.h"

namespace ECSEngine
{
namespace Rendering
{

GFXRepresentationManager::GFXRepresentationManager()
{
}

GFXRepresentationManager::~GFXRepresentationManager()
{
}

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

GFXRepresentation* GFXRepresentationManager::GetGFX(const u32 parId) const
{
    if (parId == -1)
        return nullptr;
    auto itFind = FGFXRepresentations.find(parId);
    if (itFind == FGFXRepresentations.end())
        return nullptr;

    return itFind->second.get();
}

template<typename T>
void GFXRepresentationManager::PushMessage(const u32& parId, u32 parKey, const T& parData, const float parTime)
{
    AssertRelease(parId != -1);
    auto itFind = FGFXRepresentations.find(parId);
    AssertRelease(itFind != FGFXRepresentations.end());
    AssertRelease(itFind->second != nullptr);
    FGFXRepresentations[parId]->GetCurrentQueueForPushingMessage().PushMessage(parKey, parData, parTime);
}

template void GFXRepresentationManager::PushMessage<vec3>(const u32& parId, u32 parKey, const vec3& parData, const float parTime);
template void GFXRepresentationManager::PushMessage<quat>(const u32& parId, u32 parKey, const quat& parData, const float parTime);
template void GFXRepresentationManager::PushMessage<bool>(const u32& parId, u32 parKey, const bool& parData, const float parTime);

} // namespace Rendering
} // namespace ECSEngine
