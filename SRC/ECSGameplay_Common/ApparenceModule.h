#pragma once
#include "Common/MeshHandle.h"
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{
namespace Rendering
{
class MeshHandle;
}
class ApparenceModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(PositionModule);

public:
    ApparenceModuleTemplate()
        : ModuleTemplate()
    {
    }

    virtual ~ApparenceModuleTemplate() {}

    virtual Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;
};

class ApparenceModule final : public Module
{
    DECLARE_MODULE(ApparenceModule);

public:
    ApparenceModule();
    ~ApparenceModule() {}

    const Rendering::MeshHandle& GetMeshHandle() const;
    // bgfx::ProgramHandle GetProgramHandle() const { return FProgram; }

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;
    void VirtualDeinit() override;

private:
    // bgfx::ProgramHandle FProgram;
    Rendering::MeshHandle FMeshHandle;
};
} // namespace ECSEngine