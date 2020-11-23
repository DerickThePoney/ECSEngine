#pragma once

#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class PeonFeedingTimeSystem final : public ModuleSystem
{
public:
    PeonFeedingTimeSystem();
    ~PeonFeedingTimeSystem();

protected:
    void VirtualUpdate() override;
};
} // namespace ECSEngine
