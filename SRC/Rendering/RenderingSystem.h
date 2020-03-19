#pragma once
#include "Common/Camera.h"
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

    std::weak_ptr<Camera> c;
};
} // namespace ECSEngine
