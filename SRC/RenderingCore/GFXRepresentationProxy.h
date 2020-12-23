#pragma once
#include "ECSCore/EntityId.h"
#include "GFXRepresentation.h"
#include "GFXRepresentationManager.h"

namespace ECSEngine
{
namespace Rendering
{
class GFXRepresentationProxy
{
    DECLARE_POOL_ALLOCATED(GFXRepresentationProxy);

public:
    GFXRepresentationProxy();
    ~GFXRepresentationProxy();

    void Initialise(const GFXRepresentationInitialiser& parInitialise);
    void Cleanup();

    template<typename T>
    void PushMessage(u32 parKey, const T& parData, const float parTime)
    {
        AssertRelease(FId != -1);
        GFXRepresentationManager::Instance().PushMessage(FId, parKey, parData, parTime);
    }

private:
    u32 FId = -1;
};
} // namespace Rendering
} // namespace ECSEngine
