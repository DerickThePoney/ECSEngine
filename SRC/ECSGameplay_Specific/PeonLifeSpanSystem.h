#pragma once

#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class PeonLifeSpanSystem final : public ModuleSystem
{
public:
    PeonLifeSpanSystem();
    ~PeonLifeSpanSystem();

protected:
    void VirtualUpdate() override;
};
} // namespace ECSEngine
