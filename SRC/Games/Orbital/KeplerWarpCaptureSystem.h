#pragma once
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class KeplerWarpCaptureSystem : public ModuleSystem
{
public:
    KeplerWarpCaptureSystem();

protected:
    void VirtualUpdate() override;

private:
    u32 PreviousMultiplier = 1;
};
} // namespace ECSEngine
