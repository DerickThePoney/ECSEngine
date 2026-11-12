#pragma once
#include "Common/InputCommands.h"
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class RocketControllerSystem final : public ModuleSystem
{
public:
    RocketControllerSystem();

protected:
    void VirtualUpdate() override;

    KeyboardCommand FLeft;
    KeyboardCommand FRight;
    KeyboardCommand FUp;
    KeyboardCommand FDown;

    KeyboardCommand FSASToggle;

    KeyboardCommand FIncreaseThrottle;
    KeyboardCommand FDecreaseThrottle;
    KeyboardCommand FFullThrottle;
    KeyboardCommand FCutThrottle;
};
} // namespace ECSEngine