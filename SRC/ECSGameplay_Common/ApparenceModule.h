#pragma once
#include "Common/RenderingHandles.h"
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
    DECLARE_MODULE_TEMPLATE(ApparenceModule, ApparenceModuleTemplate);

public:
    ApparenceModuleTemplate()
        : ModuleTemplate()
    {
    }

    virtual ~ApparenceModuleTemplate() {}

    virtual Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    const std::string& GetMeshFileName() const { return FMeshFileName; }

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(PROPERTY(MeshFileName));
    }

protected:
    virtual void VirtualDrawEditor() override;

private:
    std::string FMeshFileName;
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