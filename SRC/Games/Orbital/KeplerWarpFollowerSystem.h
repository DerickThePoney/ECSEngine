#pragma once
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class KeplerWarpFollowerSystem : public ModuleSystem
{
public:
    KeplerWarpFollowerSystem();

protected:
    void VirtualUpdate() override;

private:
    u32 PreviousMultiplier = 1;
};
} // namespace ECSEngine