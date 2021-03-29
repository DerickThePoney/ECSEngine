#pragma once

namespace ECSEngine
{
namespace Rendering
{
class RenderingState
{
public:
    RenderingState();
    RenderingState(const u64 initState);
    ~RenderingState();

    void PartiallyModifyState(const u64 additionalState);

    void ApplyState();

private:
    u64 FRenderingState;
    u32 FBlendingWeights;
};
} // namespace Rendering
} // namespace ECSEngine
