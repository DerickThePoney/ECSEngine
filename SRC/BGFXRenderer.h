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
};
} // namespace Rendering
} // namespace ECSEngine