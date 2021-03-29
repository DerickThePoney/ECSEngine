#pragma once
#include "Common/Camera.h"
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class SynchroWithRenderSystem final : public ModuleSystem
{
    using parent_type = ModuleSystem;

public:
    SynchroWithRenderSystem();
    ~SynchroWithRenderSystem();

protected:
    void VirtualUpdate() override;
};
} // namespace ECSEngine
