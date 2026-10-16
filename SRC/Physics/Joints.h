#pragma once
#include "Math/VectorTypes.h"
#include "PhysicsBodyHandle.h"
#include "PhysicsJointHandle.h"

namespace ECSEngine
{
namespace Physics
{
struct JointHeader
{
    PhysicsBodyHandle FBodyA;
    PhysicsBodyHandle FBodyB;

    bool bIsland = false;
};

struct JointConfig
{
    vec3 FAnchorALocal;
    vec3 FAnchorBLocal;
};

struct DistanceJointConfig
{
    JointConfig Common;
    float FLength = 0.f;
};

struct DistanceJoint
{
    JointHeader FHeader;
    DistanceJointConfig FConfig;

    float FImpulse = 0.f;

    void Initialize(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const DistanceJointConfig& Config);
    void Destroy();
};

// Spherical / ball-and-socket: point lock only
struct BallJoint
{
    JointHeader FHeader;
    vec3 FPointImpulse; // warm-start λx,λy,λz
};

// Hinge = ball point lock + axis freeze + optional limit
struct HingeJoint
{
    JointHeader FHeader;
    vec3 FPointImpulse; // same warm-start as ball
    vec3 FAxisALocal, FAxisBLocal;
    vec3 FRefALocal, FRefBLocal;
    float FReferenceAngle = 0.f;
    bool FEnableLimit = false;
    float FLowerAngle = 0.f, FUpperAngle = 0.f;
    // EHingeLimitState FLimitState = EHingeLimitState::Inactive;
    float FLimitImpulse = 0.f;
    float FAxisImpulse[2] = { };
};

} // namespace Physics
} // namespace ECSEngine