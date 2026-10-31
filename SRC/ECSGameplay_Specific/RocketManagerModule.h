
#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"
#include "Physics/JointsConfig.h"
#include "Physics/PhysicsJointHandle.h"

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

    EntityId BodyId() const { return FBodyId; }
    EntityId NozzleId() const { return FNozzleId; }

    Physics::PhysicsJointHandle NozzleJointHandle() const { return FJointHandle; }

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;

    void OnJointCreated(const Physics::PhysicsJointHandle& Handle);

private:
    EntityId FBodyId;
    EntityId FNozzleId;

    Physics::PhysicsJointHandle FJointHandle;

    Delegate<void(const Physics::PhysicsJointHandle&)> JointCreatedDelegate;
};

} // namespace ECSEngine
