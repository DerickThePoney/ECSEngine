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

void GFXRepresentationProxy::Initialise(const GFXRepresentationInitialiser& parInitialise)
{
    FId = GFXRepresentationManager::Instance().CreateGFXRepresentation(parInitialise);
    AssertRelease(FId != -1);
}

std::pair<bool, bool> GFXRepresentationProxy::IsGFXSelectedOrHighlighted() const
{
    AssertRelease(FId != -1);
    return GFXRepresentationManager::Instance().IsGFXSelectedOrHighlighted(FId);
}

void GFXRepresentationProxy::Cleanup()
{
    AssertRelease(FId != -1);
    GFXRepresentationManager::Instance().DeleteGFXRepresentation(FId);
}

} // namespace Rendering
} // namespace ECSEngine
