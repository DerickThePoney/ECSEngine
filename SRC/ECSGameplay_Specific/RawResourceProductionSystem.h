#pragma once
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class RawResourceProductionSystem final : public ModuleSystem
{
public:
    RawResourceProductionSystem();
    ~RawResourceProductionSystem();

protected:
    void VirtualUpdate() override;
};
} // namespace ECSEngine
