#pragma once
#include "Common/Singleton.h"

namespace ECSEngine
{
namespace Rendering
{
class DrawCommandBuffer;
class FinalCombinePass : public Singleton<FinalCombinePass>
{
public:
    void Initialise();
    void Shutdown();

    void SetTextures(u16 parGameTexture, u16 parUiTexture);
    void Render();

private:
    DrawCommandBuffer* FCommandBuffer = nullptr;
};
} // namespace Rendering
} // namespace ECSEngine