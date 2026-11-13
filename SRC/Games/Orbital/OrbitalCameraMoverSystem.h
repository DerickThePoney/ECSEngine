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

class OrbitalCameraMoverSystem : public ModuleSystem
{
public:
    OrbitalCameraMoverSystem();

protected:
    void VirtualUpdate() override;

private:
    void UpdateRocketChase();
    void UpdateMapMode();

    MouseButtonCommand FMiddleMouseRotation;
    MouseScrollCommand FMiddleMouseScroll;

    KeyboardCommand FMapModeChange;
    ECameraMode FCameraMode = ECameraMode::ROCKET_CHASE;

    float FStoredNear = 0.1f;
    bool FMapNearActive = false;
};
} // namespace ECSEngine
