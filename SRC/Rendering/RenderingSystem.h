#pragma once
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class RenderingSystem final : public ModuleSystem
{
    using parent_type = ModuleSystem;

public:
    RenderingSystem();
    virtual ~RenderingSystem();

protected:
    void VirtualInit() override;
    void VirtualUpdate() override;
    void VirtualDestroy() override;

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> FStart;
};
} // namespace ECSEngine
