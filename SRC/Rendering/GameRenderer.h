#pragma once
#include "Common/Singleton.h"

namespace ECSEngine
{
namespace Rendering
{
class FramebufferInstance;
class DrawCommandBuffer;
class GFXPickingRenderer;
class OutlineRenderer;

class GameRenderer : public Singleton<GameRenderer>
{
public:
    GameRenderer();
    ~GameRenderer();

    void Initialise();
    void Shutdown();

    void Render();

    u16 GetFinalTexture() const;

private:
    void SetViewFramebuffers(const uvec2 parSize);

private:
    u32 FGameplayCameraId = -1;

    FramebufferInstance* FGeometryFramebuffer = nullptr;
    FramebufferInstance* FCombineFramebuffer = nullptr;

    DrawCommandBuffer* FGeometryCommandBuffer = nullptr;
    DrawCommandBuffer* FFeedbackCommandBuffer = nullptr;
    DrawCommandBuffer* FCombineCommandBuffer = nullptr;

    // renderers
    std::unique_ptr<GFXPickingRenderer> FPickingRenderer;
    std::unique_ptr<OutlineRenderer> FOutlineRenderer;
};
} // namespace Rendering
} // namespace ECSEngine
