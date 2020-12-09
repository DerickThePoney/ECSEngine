#pragma once
#include "Common/InputCommands.h"
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class CameraMoverSystem final : public ModuleSystem
{
public:
    CameraMoverSystem();

protected:
    void VirtualUpdate() override;

    KeyboardCommand FLeft;
    KeyboardCommand FRight;
    KeyboardCommand FUp;
    KeyboardCommand FDown;

    MouseButtonCommand FMiddleMouseRotation;
    MouseScrollCommand FMiddleMouseScroll;
};
} // namespace ECSEngine