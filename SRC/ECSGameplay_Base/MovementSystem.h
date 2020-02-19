#pragma once
#include "ECSBase/ModuleSystem.h"

namespace ECSEngine
{
class MovementSystem final : public ModuleSystem
{
    using parent_type = ModuleSystem;

public:
    MovementSystem();
    virtual ~MovementSystem();

protected:
    void VirtualInit() override;
    void VirtualUpdate() override;
};
} // namespace ECSEngine
