#pragma once
#include "Common/RenderingHandles.h"

namespace ECSEngine
{
namespace Rendering
{
class GFXRepresentation;
class DrawCommandBuffer;
class FramebufferInstance;
constexpr u32 PickTextureSize = 8;
class GFXPickingRenderer
{
public:
    void Initialise();
    void Cleanup();

    void BeginSelectionPass(const u32 parCamera);
    void PushGFXForSelectionPass(const GFXRepresentation* parGFX);
    void EndSelectionPass();

    void SetDataIsAvailable();

private:
    // selectionpass rendering
    DrawCommandBuffer* FDrawCommandBuffer = nullptr;
    FramebufferInstance* FPickFramebuffer = nullptr;

    MaterialInstanceHandle FDrawIdMaterial;

    u8 FSelectionData[PickTextureSize * PickTextureSize * 4];

    float FSelectionFoV = 1.f;
    u32 FHighlightedGFXId = -1;
    u32 FPreviousFrameHighlightedGFXId = -1;

    u32 FSelectedGFXId = -1;
    u32 FPreviousFrameSelectedGFXId = -1;

    u32 FHighlithedGFXIdHits = 0;

    bool FReadingData = false;
    bool FReadingAvailable = false;
};
} // namespace Rendering
} // namespace ECSEngine