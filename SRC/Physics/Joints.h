#pragma once
#include "Math/VectorTypes.h"
#include "PhysicsBodyHandle.h"
#include "PhysicsJointHandle.h"

namespace ECSEngine
{
namespace Physics
{
class Island;

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

struct JointBase
{
    PhysicsBodyHandle FBodyA;
    PhysicsBodyHandle FBodyB;

    bool bIsland = false;

    virtual void PreSolve(Island* parIsland) = 0;
    virtual void Solve(Island* parIsland) = 0;
};

struct DistanceJoint : public JointBase
{
    DistanceJointConfig FConfig;

    float FImpulse = 0.f;

    void Initialize(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const DistanceJointConfig& Config);
    void Destroy();

    void PreSolve(Island* parIsland) override { }
    void Solve(Island* parIsland) override { }
};

// Spherical / ball-and-socket: point lock only
struct BallJoint : public JointBase
{
    vec3 FPointImpulse; // warm-start λx,λy,λz

    void PreSolve(Island* parIsland) override { }
    void Solve(Island* parIsland) override { }
};

// Hinge = ball point lock + axis freeze + optional limit
struct HingeJoint : public JointBase
{
    vec3 FPointImpulse; // same warm-start as ball
    vec3 FAxisALocal, FAxisBLocal;
    vec3 FRefALocal, FRefBLocal;
    float FReferenceAngle = 0.f;
    float FLowerAngle = 0.f, FUpperAngle = 0.f;
    // EHingeLimitState FLimitState = EHingeLimitState::Inactive;
    float FLimitImpulse = 0.f;
    float FAxisImpulse[2] = { };
    bool FEnableLimit = false;

    void PreSolve(Island* parIsland) override { }
    void Solve(Island* parIsland) override { }
};

} // namespace Physics
} // namespace ECSEngine