#pragma once
#include "Common/RenderingHandles.h"
namespace ECSEngine
{
class SceneScenario;
namespace Rendering
{
class DrawCommandBuffer;
class FramebufferInstance;
} // namespace Rendering
constexpr u32 PickTextureSize = 8;
class SceneObjectsPickingRenderer
{
public:
    SceneObjectsPickingRenderer();
    ~SceneObjectsPickingRenderer();

    void Initialise();
    void Shutdown();
    void RenderScene(const SceneScenario* parScene);

    void DrawDebugData(bool* parOpen);

    std::pair<u32, u32> GetPickedItemAndHits(const float minProportion = 0.0f) const;

    void SetDataIsAvailable();

private:
    Rendering::DrawCommandBuffer* FDrawCommandBuffer;
    Rendering::FramebufferInstance* FPickFramebuffer;

    Rendering::MaterialInstanceHandle FDrawIdMaterial;

    u8 FSelectionData[PickTextureSize * PickTextureSize * 4];

    float FSelectionFoV;
    u32 FSelectedSceneItem;
    u32 FSelectedSceneItemHits;

    bool FReadingData;
    bool FReadingAvailable;
};
} // namespace ECSEngine
