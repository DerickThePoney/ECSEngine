#pragma once
#include "Common/RenderingHandles.h"

namespace ECSEngine
{
namespace Rendering
{
class DrawCommandBuffer;
}

class EditorGridRenderer
{
public:
    EditorGridRenderer();
    ~EditorGridRenderer();

    void Initialise();
    void Shutdown();
    void RenderScene();

private:
    Rendering::DrawCommandBuffer* FDrawCommandBuffer;

    Rendering::MaterialInstanceHandle FGridMaterial;
};
} // namespace ECSEngine
