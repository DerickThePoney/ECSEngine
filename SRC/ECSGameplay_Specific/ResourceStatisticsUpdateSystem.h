#pragma once
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class ResourceStatisticsUpdateSystem final : public ModuleSystem
{
public:
    ResourceStatisticsUpdateSystem();

protected:
    void VirtualUpdate() override;
};
} // namespace ECSEngine