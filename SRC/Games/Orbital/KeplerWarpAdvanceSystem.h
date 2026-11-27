#pragma once
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class KeplerWarpAdvanceSystem : public ModuleSystem
{
public:
    KeplerWarpAdvanceSystem();

    void ExternalAdvanceBy(const double DT);

protected:
    void VirtualUpdate() override;

    void AdvanceBy(const double DT);
};
} // namespace ECSEngine
