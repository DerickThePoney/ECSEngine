#pragma once
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class PeonSpawnSystem : public ModuleSystem
{
public:
    PeonSpawnSystem();
    ~PeonSpawnSystem();

protected:
    void VirtualUpdate() override;
};
} // namespace ECSEngine
