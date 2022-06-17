#pragma once
#include "Common/IdGenerator.h"
#include "Common/Singleton.h"

namespace ECSEngine
{
namespace Rendering
{
class GFXRepresentation;
struct GFXRepresentationInitialiser;
class GFXRepresentationManager : public Singleton<GFXRepresentationManager>
{
public:
    GFXRepresentationManager();
    ~GFXRepresentationManager();
    void Update(float parCurrentTime);

    void OnGameplayFrameEnded();

    u32 CreateGFXRepresentation(const GFXRepresentationInitialiser& parInit);
    void DeleteGFXRepresentation(const u32 parId);
    std::pair<bool, bool> IsGFXSelectedOrHighlighted(const u32 parId) const;

    template<typename T>
    void PushMessage(const u32& parId, u32 parKey, const T& parData, const float parTime);

    std::map<u32, std::unique_ptr<GFXRepresentation>>::const_iterator begin() const { return FGFXRepresentations.begin(); }
    std::map<u32, std::unique_ptr<GFXRepresentation>>::const_iterator end() const { return FGFXRepresentations.end(); }
    GFXRepresentation* GetGFX(const u32 parId) const;

private:
    IdGenerator FGFXIdGenerator;
    std::map<u32, std::unique_ptr<GFXRepresentation>> FGFXRepresentations;

    std::atomic_bool FGameplayFrameEnded = false;
    mutable std::mutex FMutex;
};

} // namespace Rendering
} // namespace ECSEngine
