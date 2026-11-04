
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
        PROPERTYFIELD(RocketCameraMoverTemplateName, "");
        PROPERTYFIELD(BallConfig, Physics::BallJointConfig{ })
    }

    const EntityTemplate* GetBodyTemplate() const { return FRocketBodyTemplate; }
    const EntityTemplate* GetNozzleTemplate() const { return FRocketNozzleTemplate; }
    const EntityTemplate* GetRocketCameraMoverTemplate() const { return FRocketCameraMoverTemplate; }
    const Physics::BallJointConfig& GetJointConfig() const { return FBallConfig; }

protected:
    void VirtualDrawEditor() override;
    void VirtualPostLoad() override;

private:
    std::string FRocketBodyTemplateName;
    std::string FRocketNozzleTemplateName;
    std::string FRocketCameraMoverTemplateName;

    const EntityTemplate* FRocketBodyTemplate = nullptr;
    const EntityTemplate* FRocketNozzleTemplate = nullptr;
    const EntityTemplate* FRocketCameraMoverTemplate = nullptr;

    Physics::BallJointConfig FBallConfig;
};

enum class ESASMode : u8
{
    STABILITY,
    PROGRADE,
    RETROGRADE,
    RADIALOUT,
    RADIALIN,
    NORMAL,
    ANTINORMAL
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

    const vec3& LastTurnGravity() const { return FLastTurnGravity; }
    void SetLastTurnGravity(const vec3& Gravity) { FLastTurnGravity = Gravity; }

    const bool EngineFiring() const { return FEngineFiring; }
    void SetEngineFiring(const bool Value) { FEngineFiring = Value; }

    const bool IsSteering() const { return FIsSteering; }
    void SetIsSteering(const bool Value) { FIsSteering = Value; }

    const vec3& TargetSteeringAngle() const { return FTargetSteeringAngle; }
    void SetTargetSteeringAngle(const vec3& Value) { FTargetSteeringAngle = Value; }

    bool SASActivated() const { return FSASActivated; }
    bool WasSASActivated() const { return FWasSASActivated; }
    void SetSASActivated(const bool Value);

    bool HasSASTarget() const { return FHasSASTarget; }
    const quat& SASTarget() const { return FSASTarget; }
    void ClearSASTarget() { FHasSASTarget = false; }
    void SetSASTarget(const quat& Target);

    ESASMode SASMode() const { return FSASMode; }
    void SetSASMode(const ESASMode mode) { FSASMode = mode; }

    Physics::PhysicsJointHandle NozzleJointHandle() const { return FJointHandle; }

    template<typename RigidbodyAccessor>
    bool ComputeCenterOfMass(const RigidbodyAccessor& Rigidbodies, vec3& OutPosition, vec3* OutVelocity = nullptr) const;

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;

    void OnJointCreated(const Physics::PhysicsJointHandle& Handle);

private:
    EntityId FBodyId;
    EntityId FNozzleId;

    Physics::PhysicsJointHandle FJointHandle;

    vec3 FLastTurnGravity = vec3(0.f, -1.f, 0.f);

    vec3 FTargetSteeringAngle;

    quat FSASTarget;
    ESASMode FSASMode = ESASMode::STABILITY;
    bool FSASActivated = false;
    bool FWasSASActivated = false;
    bool FHasSASTarget = false;

    bool FEngineFiring = false;
    bool FIsSteering = false;

    Delegate<void(const Physics::PhysicsJointHandle&)> JointCreatedDelegate;
};

} // namespace ECSEngine
