#pragma once
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class KeplerWarpAdvanceSystem : public ModuleSystem
{
public:
    KeplerWarpAdvanceSystem();

protected:
    void VirtualUpdate() override;
};
} // namespace ECSEngine
