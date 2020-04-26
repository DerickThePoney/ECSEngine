#include "stdafx.h"

#include "RenderingState.h"

namespace ECSEngine
{
namespace Rendering
{

RenderingState::RenderingState()
    : FRenderingState(BGFX_STATE_DEFAULT)
    , FBlendingWeights(0)
{
}

RenderingState::~RenderingState()
{
}

void RenderingState::PartiallyModifyState(const u64 additionalState)
{
    FRenderingState = FRenderingState | additionalState;
}

void RenderingState::ApplyState()
{
    bgfx::setState(FRenderingState, FBlendingWeights);
}

} // namespace Rendering
} // namespace ECSEngine