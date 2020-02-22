#pragma once
#include "Common/Singleton.h"

namespace ECSEngine
{
namespace Rendering
{
class BGFXRenderer final : public Singleton<BGFXRenderer>
{
public:
    BGFXRenderer();
    ~BGFXRenderer();

    void Init();
    void RenderFrame();
    void Shutdown();

    void Resize(u32 width, u32 height);

    bool IsInstancingEnabled();
};

} // namespace Rendering
} // namespace ECSEngine