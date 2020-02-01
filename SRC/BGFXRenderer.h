#pragma once
#include "Singleton.h"

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
};

} // namespace Rendering
} // namespace ECSEngine