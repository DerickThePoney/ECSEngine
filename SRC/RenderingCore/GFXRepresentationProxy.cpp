#include "stdafx.h"

#include "GFXRepresentationProxy.h"

namespace ECSEngine
{
namespace Rendering
{
IMPLEMENT_POOL_ALLOCATED(GFXRepresentationProxy);
GFXRepresentationProxy::GFXRepresentationProxy()
{
}

GFXRepresentationProxy::~GFXRepresentationProxy()
{
    Cleanup();
}

void GFXRepresentationProxy::Initialise(const EntityId& parUnitId, const GFXRepresentationInitialiser& parInitialise)
{
    AssertRelease(parUnitId != EntityId());
    FUnitId = parUnitId;
    GFXRepresentationManager::Instance().CreateGFXRepresentation(FUnitId, parInitialise);
}

void GFXRepresentationProxy::Cleanup()
{
    GFXRepresentationManager::Instance().DeleteGFXRepresentation(FUnitId);
}

} // namespace Rendering
} // namespace ECSEngine