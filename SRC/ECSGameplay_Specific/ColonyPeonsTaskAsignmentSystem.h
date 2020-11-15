#pragma once
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class ColonyPeonsTaskAssignmentSystem : public ModuleSystem
{
public:
    ColonyPeonsTaskAssignmentSystem();
    ~ColonyPeonsTaskAssignmentSystem();

protected:
    void VirtualUpdate() override;
};
} // namespace ECSEngine
