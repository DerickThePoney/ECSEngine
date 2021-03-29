#pragma once
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
namespace Rendering
{
class DrawCommandBuffer;
class MaterialInstanceHandle;
} // namespace Rendering
class MovementSystem final : public ModuleSystem
{
    using parent_type = ModuleSystem;

public:
    MovementSystem();
    virtual ~MovementSystem();

    void VisualDebug(Rendering::DrawCommandBuffer& parBuffer, const Rendering::MaterialInstanceHandle& parMaterial);

protected:
    void VirtualInit() override;
    void VirtualUpdate() override;
};
} // namespace ECSEngine
