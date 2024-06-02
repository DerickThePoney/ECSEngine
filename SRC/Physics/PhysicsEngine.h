#pragma once
#include "Common/IdGenerator.h"
#include "Common/Singleton.h"
#include "PhysicsEngineConfiguration.h"
#include "RigidBody.h"

namespace ECSEngine
{
namespace Physics
{
struct PhysicsBodyHandle;
struct PhysicsBodyConfig;

class PhysicsEngine : public Singleton<PhysicsEngine>
{
public:
    PhysicsEngine();
    ~PhysicsEngine();

    void Initialize(const PhysicsEngineConfiguration& PhysicsConfig);
    void Cleanup();

    void UpdatePhysics(float parDeltaTime);

    const PhysicsEngineConfiguration& Config() const { return FConfig; }
    void SetConfig(const PhysicsEngineConfiguration& NewConfig) { FConfig = NewConfig; }

    const PhysicsBodyHandle CreateNewPhysicsBody(const mat4& Transform, const PhysicsBodyConfig& BodyConfig);
    bool DestroyPhysicsBody(const PhysicsBodyHandle& Handle);

    // body manipulation
    bool SetBodyPosition(const PhysicsBodyHandle& Handle, const vec3& Position);
    bool SetBodyVelocity(const PhysicsBodyHandle& Handle, const vec3& Velocity);
    bool SetBodyOrientation(const PhysicsBodyHandle& Handle, const quat& Orientation);
    bool SetBodyRotationVelocity(const PhysicsBodyHandle& Handle, const vec3& RotationVelocity);

    bool GetBodyPosition(const PhysicsBodyHandle& Handle, vec3& Position) const;
    bool GetBodyVelocity(const PhysicsBodyHandle& Handle, vec3& Velocity) const;
    bool GetBodyOrientation(const PhysicsBodyHandle& Handle, quat& Orientation) const;
    bool GetBodyRotationVelocity(const PhysicsBodyHandle& Handle, vec3& RotationVelocity) const;

    // body forces and torques
    bool AddForceToBody(const PhysicsBodyHandle& Handle, const vec3& Force, bool bTreatAsAcceleration);
    bool AddForceAtPointToBody(const PhysicsBodyHandle& Handle, const vec3& Force, const vec3& Point, bool bTreatPointAsLocal, bool bTreatAsAcceleration);

    bool AddTorqueToBody(const PhysicsBodyHandle& Handle, const vec3& Torque);

    bool AddImpulseToBody(const PhysicsBodyHandle& Handle, const vec3& Impulse, bool bTreatAsVelocityChange);

private:
    void InitializeBody(RigidBody* Body, const mat4& Transform, const PhysicsBodyConfig& BodyConfig);

    void UpdateInertiaTransform(RigidBody* Body);

    RigidBody* GetRigidBody(const PhysicsBodyHandle& Handle);
    const RigidBody* GetRigidBody(const PhysicsBodyHandle& Handle) const;

private:
    PhysicsEngineConfiguration FConfig;

    IdGenerator FHandleGenerator; // TODO MAKE PHYSICS HANDLE ID GENERATOR THAT SPECIALISES THIS
    std::vector<std::unique_ptr<RigidBody>> FRigidbodies;
};
} // namespace Physics
} // namespace ECSEngine