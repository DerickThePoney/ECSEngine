#pragma once
#include "Common/InputCommands.h"
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class RocketCameraMoverSystem : public ModuleSystem
{
public:
    RocketCameraMoverSystem();

protected:
    void VirtualUpdate() override;

private:
    MouseButtonCommand FMiddleMouseRotation;
    MouseScrollCommand FMiddleMouseScroll;
};
} // namespace ECSEngine