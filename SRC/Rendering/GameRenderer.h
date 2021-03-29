#pragma once
#include "Common/Singleton.h"

namespace ECSEngine
{
namespace Rendering
{
class FramebufferInstance;
class DrawCommandBuffer;

class GameRenderer : public Singleton<GameRenderer>
{
public:
    GameRenderer()
        : Singleton()
    {
    }
    ~GameRenderer() { }

    void Initialise();
    void Shutdown();

    void Render();

private:
    void SetViewFramebuffers(const glm::uvec2 parSize);

private:
    u32 FGameplayCameraId = -1;

    FramebufferInstance* FGeometryFramebuffer = nullptr;

    DrawCommandBuffer* FGeometryCommandBuffer = nullptr;
    DrawCommandBuffer* FFeedbackCommandBuffer = nullptr;
    DrawCommandBuffer* FCombineCommandBuffer = nullptr;
};
} // namespace Rendering
} // namespace ECSEngine
