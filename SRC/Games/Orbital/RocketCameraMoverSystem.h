#pragma once
#include "Common/InputCommands.h"
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
enum class ECameraMode : u8
{
    ROCKET_CHASE,
    MAP_MODE,
    LENGTH
};

class RocketCameraMoverSystem : public ModuleSystem
{
public:
    RocketCameraMoverSystem();

protected:
    void VirtualUpdate() override;

private:
    MouseButtonCommand FMiddleMouseRotation;
    MouseScrollCommand FMiddleMouseScroll;

    KeyboardCommand FMapModeChange;
    ECameraMode FCameraMode = ECameraMode::ROCKET_CHASE;
};
} // namespace ECSEngine