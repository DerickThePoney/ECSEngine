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

    void Initialise(const EntityId& parUnitId, const GFXRepresentationInitialiser& parInitialise);
    void Cleanup();

    template<typename T>
    void PushMessage(u32 parKey, const T& parData, const float parTime)
    {
        GFXRepresentationManager::Instance().PushMessage(FUnitId, parKey, parData, parTime);
    }

private:
    EntityId FUnitId;
};
} // namespace Rendering
} // namespace ECSEngine
