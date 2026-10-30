
#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"
#include "Physics/JointsConfig.h"

namespace ECSEngine
{
class RocketManagerModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(RocketManagerModule, RocketManagerModuleTemplate);

public:
    RocketManagerModuleTemplate()
        : ModuleTemplate()
    {
    }
    ~RocketManagerModuleTemplate() { }

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    SERIALIZE()
    {
        PROPERTYFIELD(RocketBodyTemplateName, "");
        PROPERTYFIELD(RocketNozzleTemplateName, "");
        PROPERTYFIELD(BallConfig, Physics::BallJointConfig{ })
    }

    const EntityTemplate* GetBodyTemplate() const { return FRocketBodyTemplate; }
    const EntityTemplate* GetNozzleTemplate() const { return FRocketNozzleTemplate; }
    const Physics::BallJointConfig& GetJointConfig() const { return FBallConfig; }

protected:
    void VirtualDrawEditor() override;
    void VirtualPostLoad() override;

private:
    std::string FRocketBodyTemplateName;
    std::string FRocketNozzleTemplateName;

    const EntityTemplate* FRocketBodyTemplate = nullptr;
    const EntityTemplate* FRocketNozzleTemplate = nullptr;

    Physics::BallJointConfig FBallConfig;
};

class RocketManagerModule : public Module
{
    DECLARE_MODULE(RocketManagerModule);

    DECLARE_SAVELOAD_ABILITIES();

public:
    RocketManagerModule();
    ~RocketManagerModule();

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;

private:
    EntityId FBodyId;
    EntityId FNozzleId;
};

} // namespace ECSEngine
