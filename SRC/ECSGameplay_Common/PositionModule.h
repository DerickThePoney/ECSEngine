#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{

class PositionModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(PositionModule, PositionModuleTemplate);

public:
    PositionModuleTemplate()
        : ModuleTemplate()
    {
    }
    virtual ~PositionModuleTemplate() { }

    virtual Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

protected:
    virtual void VirtualDrawEditor() override;
};

class PositionModule final : public Module
{
    DECLARE_MODULE(PositionModule);

public:
    PositionModule()
        : Module()
        , FPosition(0.0f)
    {
    }

    const glm::vec3& GetPosition3D() const { return FPosition; }
    void SetPosition3D(const glm::vec3& parPosition) { FPosition = parPosition; }

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;

private:
    glm::vec3 FPosition;
};
} // namespace ECSEngine