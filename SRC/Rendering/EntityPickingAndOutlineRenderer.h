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
class EntityPickingAndOutlineRenderer
{
public:
    void Initialise();
    void Cleanup();

    void BeginSelectionPass(const u32 parCamera);
    void PushGFXForSelectionPass(const GFXRepresentation* parGFX);
    void EndSelectionPass();
    void UpdateAndRender();

    void SetDataIsAvailable();

private:
    // push entity for selection pass

    // submit selection pass stuffs

    // render outline IFN

private:
    // selectionpass rendering
    DrawCommandBuffer* FDrawCommandBuffer = nullptr;
    FramebufferInstance* FPickFramebuffer = nullptr;

    MaterialInstanceHandle FDrawIdMaterial;

    u8 FSelectionData[PickTextureSize * PickTextureSize * 4];

    float FSelectionFoV = 1.f;
    u32 FSelectedEntity = -1;
    u32 FSelectedEntityHits = 0;

    bool FReadingData = false;
    bool FReadingAvailable = false;
};
} // namespace Rendering
} // namespace ECSEngine