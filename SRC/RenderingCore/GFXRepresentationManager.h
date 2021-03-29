#pragma once
#include "Common/IdGenerator.h"
#include "Common/Singleton.h"
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

    u32 CreateGFXRepresentation(const GFXRepresentationInitialiser& parInit);
    void DeleteGFXRepresentation(const u32 parId);

    template<typename T>
    void PushMessage(const u32& parId, u32 parKey, const T& parData, const float parTime)
    {
        AssertRelease(parId != -1);
        auto itFind = FGFXRepresentations.find(parId);
        AssertRelease(itFind != FGFXRepresentations.end());
        AssertRelease(itFind->second != nullptr);
        FGFXRepresentations[parId]->GetCurrentQueueForPushingMessage().PushMessage(parKey, parData, parTime);
    }

    std::map<u32, std::unique_ptr<GFXRepresentation>>::const_iterator begin() const { return FGFXRepresentations.begin(); }
    std::map<u32, std::unique_ptr<GFXRepresentation>>::const_iterator end() const { return FGFXRepresentations.end(); }

private:
    IdGenerator FGFXIdGenerator;
    std::map<u32, std::unique_ptr<GFXRepresentation>> FGFXRepresentations;

    std::atomic_bool FGameplayFrameEnded = false;
    std::mutex FMutex;
};
} // namespace Rendering
} // namespace ECSEngine
