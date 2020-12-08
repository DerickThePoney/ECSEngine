#pragma once
#include "Common/Singleton.h"
#include "ECSCore/EntityId.h"
#include "GFXRepresentation.h"

namespace ECSEngine
{
namespace Rendering
{
class GFXRepresentationManager : public Singleton<GFXRepresentationManager>
{
public:
    void Update(float parCurrentTime);

    void OnGameplayFrameEnded();

    void CreateGFXRepresentation(const EntityId& parId, const GFXRepresentationInitialiser& parInit);
    void DeleteGFXRepresentation(const EntityId& parId);

    template<typename T>
    void PushMessage(const EntityId& parId, u32 parKey, const T& parData, const float parTime)
    {
        AssertRelease(parId != EntityId());
        auto itFind = FGFXRepresentations.find(parId);
        AssertRelease(itFind != FGFXRepresentations.end());
        itFind->second->GetCurrentQueueForPushingMessage().PushMessage(parKey, parData, parTime);
    }

private:
    std::map<EntityId, std::unique_ptr<GFXRepresentation>> FGFXRepresentations;

    std::atomic_bool FGameplayFrameEnded = false;
    std::mutex FMutex;
};
} // namespace Rendering
} // namespace ECSEngine