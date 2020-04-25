#pragma once
#include "Common/RenderingHandles.h"
namespace ECSEngine
{
class Scene;
namespace Rendering
{
class DrawCommandBuffer;
}
constexpr u32 PickTextureSize = 8;
class SceneObjectsPickingRenderer
{
public:
    SceneObjectsPickingRenderer();
    ~SceneObjectsPickingRenderer();

    void Initialise();
    void Shutdown();
    void RenderScene(const Scene* parScene);

    void SetDataIsAvailable();

private:
    Rendering::DrawCommandBuffer* FDrawCommandBuffer;
    Rendering::DrawCommandBuffer* FBlitCommandBuffer;

    Rendering::MaterialInstanceHandle FDrawIdMaterial;

    u8 FSelectionData[PickTextureSize * PickTextureSize * 4];

    u32 FSelectionDataTimestamp;

    bool FReadingData;
    bool FReadingAvailable;
};
} // namespace ECSEngine
