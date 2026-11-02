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
    void VirtualInit() override;
    void VirtualUpdate() override;

private:
    MouseButtonCommand FMiddleMouseRotation;
    MouseScrollCommand FMiddleMouseScroll;

    // Temporary Hacks --> TODO: CREATE A SPECIFIC CAMERA MODULE FOR THIS
    bool FOrbitInitialised = false;
    float FDistance = 0.f;
    float FPitch = 0.f;
    float FYaw = 0.f;
};
} // namespace ECSEngine