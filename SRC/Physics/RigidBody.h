#pragma once
#include "CollisionShape.h"
#include "Common/BitSet.h"
#include "Math/VectorTypes.h"
#include "PhysicsBodyHandle.h"
#include "PhysicsMoveabilityEnum.h"

namespace ECSEngine
{
namespace Physics
{
enum ERigidBodyFlag : u32
{
    RB_AWAKE,
    RB_ISLAND,
    RB_COUNT
};

struct ContactEdge;
struct RigidBody
{
    DECLARE_POOL_ALLOCATED(RigidBody);

public:
    PhysicsBodyHandle FHandle;

    // Moveability
    EPhysicsMoveability::Type FMoveabilityType = EPhysicsMoveability::STATIC;

    // Collision shape
    std::vector<CollisionShape> FCollisionShapes;

    // positional stuff
    vec3 FPosition;
    vec3 FVelocity;
    vec3 FAccelerationDueToForces;

    // angular stuff
    quat FOrientation;
    vec3 FRotationVelocity;
    vec3 FTorque;

    // COM
    vec3 FCenterOfMassLocal;
    vec3 FCenterOfMassWorld;

    // Mass and inertia
    mat3 FInertiaTensor;
    mat3 FInverseInertiaTensor;
    mat3 FInverseInertiaTensorWorld;
    float FMass = 0.f;
    float FInvMass = 1.f;

    float FGravityScale = 1.f;

    float FLinearDamping = 0.1f;
    float FAngularDamping = 0.01f;

    // Restitution and friction
    float FRestitution = 0.2f;
    float FFriction = 0.4f;

    // Collision filtering
    u32 FCollisionCategory = 1;
    u32 FCollisionMask = ~0u;

    // Misc stuff
    i32 FIslandIndex = -1;
    float FSleepingTimer;

    ContactEdge* FContactList = nullptr;

    // Helper functions
    void AddForce(const vec3& Force, bool bTreatAsAcceleration);
    void AddTorque(const vec3& Torque);

    void AddImpulse(const vec3& Impulse, bool bTreatAsVelocityChange);
    void AddRotationImpulse(const vec3& Impulse, bool bTreatAsRotationVelocityChange);

    mat4 GetTransform() const;

    void SetAwake(bool bValue);
    bool IsAwake() const;

    AABB3f ComputeAABB(const mat4& parTransform) const;
    void UpdateCoMWorld();

    BitSet<ERigidBodyFlag::RB_COUNT> FFlags;
};
} // namespace Physics
} // namespace ECSEngine