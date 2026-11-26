
#pragma once
#include "Common/MemoryView.h"
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"
#include "Physics/JointsConfig.h"
#include "Physics/PhysicsJointHandle.h"
#include "RocketPartRuntime.h"
#include "RocketRecipe.h"

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
        PROPERTYFIELD(Recipe, RocketRecipe{ });
        PROPERTYFIELD(RocketCameraMoverTemplateName, "");
        PROPERTYFIELD(AxialDragArea, 0.f);
        PROPERTYFIELD(BroadsideDragArea, 0.f);
        PROPERTYFIELD(Weathercock, 0.f);
    }

    const RocketRecipe& Recipe() const { return FRecipe; }

    const EntityTemplate* GetRocketCameraMoverTemplate() const { return FRocketCameraMoverTemplate; }

    float AxialDragArea() const { return FAxialDragArea; }
    float BroadsideDragArea() const { return FBroadsideDragArea; }
    float Weathercock() const { return FWeathercock; }

protected:
    void VirtualDrawEditor() override;
    void VirtualPostLoad() override;

private:
    RocketRecipe FRecipe;

    std::string FRocketCameraMoverTemplateName;
    const EntityTemplate* FRocketCameraMoverTemplate = nullptr;

    float FAxialDragArea = 0.f;
    float FBroadsideDragArea = 0.f;
    float FWeathercock = 0.f;
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

    const vec3& LastTurnGravity() const { return FLastTurnGravity; }
    void SetLastTurnGravity(const vec3& Gravity) { FLastTurnGravity = Gravity; }

    const bool EngineFiring() const { return FRocketThrottle > 0.f; }
    float RocketThrottle() const { return FRocketThrottle; }
    void SetRocketThrottle(const float Value);

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
    void SetSASMode(const ESASMode mode)
    {
        if (FSASMode == mode)
            return;
        FSASMode = mode;
        ClearSASTarget();
    }

    const EntityId& CurrentGravityTarget() const { return FCurrentGravityTarget; }
    void SetCurrentGravityTarget(const EntityId& Value) { FCurrentGravityTarget = Value; }

    float AxialDragArea() const;
    float BroadsideDragArea() const;
    float Weathercock() const;

    u32 ActivePartsSize() const { return FActive.FParts.size(); }

    const EntityId ActiveEngine() const;
    const EntityId ActiveEngineFuelTank() const;
    const Physics::PhysicsJointHandle ActiveEngineJointHandle() const;
    const EntityId RootId() const;

    bool WantsStaging() const { return FbWantsStaging; }
    void SetStaging(bool Value) { FbWantsStaging = Value; }

    template<typename RigidbodyAccessor>
    bool Stage(const RigidbodyAccessor& Rigidbodies);

    u32 DebrisCount() const { return FDebris.size(); }

    template<typename RigidbodyAccessor>
    void SetPositionAndVelocityForAllPartsUsingOffsets(const RigidbodyAccessor& Rigidbodies,
          const vec3& CoMPosition,
          const vec3& CoMVelocity,
          const MemoryView<const vec3>& parts) const;

    template<typename RigidbodyAccessor>
    void ComputePartOffsetAndClearRotationVelocity(const RigidbodyAccessor& Rigidbodies, const vec3& CoMPosition, MemoryView<vec3>& parts) const;

    template<typename RigidbodyAccessor>
    bool ComputeActiveCenterOfMass(const RigidbodyAccessor& Rigidbodies, vec3& OutPosition, vec3* OutVelocity = nullptr) const;

    template<typename RigidbodyAccessor>
    bool ComputeCenterOfMassInDebrisIndex(const RigidbodyAccessor& Rigidbodies, u32 DebrisIndex, vec3& OutPosition, vec3* OutVelocity = nullptr) const;

    template<typename RigidbodyAccessor>
    bool ComputeActiveTotalMass(const RigidbodyAccessor& Rigidbodies, float& OutMass) const;

    template<typename RigidbodyAccessor>
    bool ComputeTotalMassInDebrisIndex(const RigidbodyAccessor& Rigidbodies, u32 DebrisIndex, float& OutMass) const;

    template<typename RigidbodyAccessor>
    bool ApplyForceToAllActiveBodies(const RigidbodyAccessor& Rigidbodies, const vec3& Force, const bool bTreatAsAcceleration) const;

    template<typename RigidbodyAccessor>
    bool ApplyForceToAllBodiesInDebrisIndex(const RigidbodyAccessor& Rigidbodies, u32 DebrisIndex, const vec3& Force, const bool bTreatAsAcceleration) const;

    template<typename RigidbodyAccessor>
    bool ApplyImpulseToAllActiveBodies(const RigidbodyAccessor& Rigidbodies, const vec3& Impulse, const bool bTreatAsVelocityChange) const;

    template<typename RigidbodyAccessor>
    bool ApplyImpulseToAllBodiesInDebrisIndex(const RigidbodyAccessor& Rigidbodies, u32 DebrisIndex, const vec3& Impulse, const bool bTreatAsVelocityChange) const;

private:
    template<typename RigidbodyAccessor>
    bool ComputeCenterOfMassInGroup(const RigidbodyAccessor& Rigidbodies, const RocketPartGroup& Group, vec3& OutPosition, vec3* OutVelocity) const;

    template<typename RigidbodyAccessor>
    bool ComputeTotalMassInGroup(const RigidbodyAccessor& Rigidbodies, const RocketPartGroup& Group, float& OutMass) const;

    template<typename RigidbodyAccessor>
    bool ApplyForceToAllBodiesInGroup(const RigidbodyAccessor& Rigidbodies, const RocketPartGroup& Group, const vec3& Force, const bool bTreatAsAcceleration) const;

    template<typename RigidbodyAccessor>
    bool ApplyImpulseToAllBodiesInGroup(const RigidbodyAccessor& Rigidbodies, const RocketPartGroup& Group, const vec3& Impulse, const bool bTreatAsVelocityChange) const;

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;

    i32 ActiveEnginePartIndex() const;
    i32 NextDecouplerIndex() const;

private:
    RocketPartGroup FActive;
    std::vector<RocketPartGroup> FDebris;

    vec3 FLastTurnGravity = vec3(0.f, -1.f, 0.f);

    vec3 FTargetSteeringAngle;

    quat FSASTarget;
    ESASMode FSASMode = ESASMode::STABILITY;

    float FRocketThrottle = 0.f;

    EntityId FCurrentGravityTarget;

    bool FSASActivated = false;
    bool FWasSASActivated = false;
    bool FHasSASTarget = false;

    bool FIsSteering = false;
    bool FbWantsStaging = false;
};

} // namespace ECSEngine
