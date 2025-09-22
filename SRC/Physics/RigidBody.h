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
    CollisionShape FCollisionShape;

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
    /*vec3 FCenterOfMassWorld;*/

    // Mass and inertia
    mat3 FInertiaTensor;
    mat3 FInverseInertiaTensor;
    mat3 FInverseInertiaTensorWorld;
    float FMass = 0.f;
    float FInvMass = 1.f;

    float FGravityScale = 1.f;

    float FLinearDamping = 0.1f;
    float FAngularDamping = 0.01f;

    ContactEdge* FContactList = nullptr;

    // Helper functions
    void AddForce(const vec3& Force, bool bTreatAsAcceleration);
    void AddTorque(const vec3& Torque);

    void AddImpulse(const vec3& Impulse, bool bTreatAsVelocityChange);
    void AddRotationImpulse(const vec3& Impulse, bool bTreatAsRotationVelocityChange);

    mat4 GetTransform() const;

    BitSet<ERigidBodyFlag::RB_COUNT> FFlags;
};
} // namespace Physics
} // namespace ECSEngine