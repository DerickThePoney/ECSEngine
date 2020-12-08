#include "stdafx.h"

#include "GFXRepresentation.h"

#include "Carrier.h"
#include "VisualModel.h"

namespace ECSEngine
{
namespace Rendering
{
IMPLEMENT_POOL_ALLOCATED(GFXRepresentation);
GFXRepresentation::GFXRepresentation()
{
}

GFXRepresentation::~GFXRepresentation()
{
    delete FCarrier;
    delete FVisualModel;
}

void GFXRepresentation::Initialise(const GFXRepresentationInitialiser& parInit)
{
    if (parInit.HasCarier)
    {
        FCarrier = new Carrier();
        AssertRelease(FCarrier != nullptr);
        FCarrier->Init(parInit.FPosition, parInit.FOrientation, parInit.FCurrentTime);
    }

    if (parInit.HasVisuals)
    {
        FVisualModel = new VisualModel();
    }
}

void GFXRepresentation::Update(float parCurrentTime)
{
    if (FMessages[FCurrentMessageQueue].HasMessages())
    {
        // ProcessMessages(parCurrentTime) -- TODO
        FMessages[FCurrentMessageQueue].ClearMessages();
    }

    if (FCarrier != nullptr)
        FCarrier->Update(parCurrentTime);

    if (FVisualModel != nullptr)
    {
        /// DOSTUFF ON UPDATE IFN? ANIMATIONS UPDATE?
    }
}

void GFXRepresentation::SwapQueues()
{
    FCurrentMessageQueue = 1 - FCurrentMessageQueue;
}
} // namespace Rendering
} // namespace ECSEngine
