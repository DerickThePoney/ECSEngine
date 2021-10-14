#pragma once
#include "Common/MemoryView.h"
#include "Common/RenderingHandles.h"

namespace ECSEngine
{
namespace Rendering
{
class FramebufferInstance;
class GFXRepresentation;
class DrawCommandBuffer;
class OutlineRenderer
{
public:
    void Initialise();
    void Cleanup();

    void RenderOutline(MemoryView<const GFXRepresentation*> parSelectedRepresentations, MemoryView<const GFXRepresentation*> parHighlightedRepresentations);

    u16 GetTextureHandle() const;

private:
    void AddGFXForOutline(const GFXRepresentation* parRepresentation, bool parSelected);

private:
    DrawCommandBuffer* FDrawBuffer = nullptr;
    DrawCommandBuffer* FSolidDrawBuffer = nullptr;
    DrawCommandBuffer* FHorizontalDrawBuffer = nullptr;
    DrawCommandBuffer* FVerticalDrawBuffer = nullptr;
    FramebufferInstance* FInitialFramebuffer = nullptr;
    FramebufferInstance* FIntermediaryFramebuffer = nullptr;

    MaterialInstanceHandle FDrawIdMaterial;
    MaterialInstanceHandle FSolidFilter;
    MaterialInstanceHandle FHorizontalFilter;
    MaterialInstanceHandle FVerticalFilter;

    u32 FGameplayCameraId = -1;
};
} // namespace Rendering
} // namespace ECSEngine