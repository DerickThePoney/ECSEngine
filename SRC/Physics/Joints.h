#pragma once
#include "JointsConfig.h"
#include "Math/VectorTypes.h"
#include "PhysicsBodyHandle.h"
#include "PhysicsJointHandle.h"

namespace ECSEngine
{
namespace Physics
{
class Island;

struct JointBase
{
    PhysicsBodyHandle FBodyA;
    PhysicsBodyHandle FBodyB;

    bool bIsland = false;

    virtual void PreSolve(Island* parIsland, const float parDT) = 0;
    virtual void Solve(Island* parIsland, const float parDT) = 0;
};

struct DistanceJointState
{
    vec3 xA, xB;
    vec3 rA, rB;
    vec3 d, n;
    float C = 0.f;
    float M_eff = 0.f;
    float bias = 0.f;
};

struct DistanceJoint : public JointBase
{
    DistanceJointConfig FConfig;

    DistanceJointState FState; // TODO MOVE THIS OUT IN A SEPARATE SOLVER

    float FImpulse = 0.f;

    void Initialize(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const DistanceJointConfig& Config);
    void Destroy();

    void PreSolve(Island* parIsland, const float parDT) override;
    void Solve(Island* parIsland, const float parDT) override;
};

struct BallJointState
{
    vec3 xA, xB;
    vec3 rA, rB;
    vec3 M_eff;
    vec3 bias;
    vec3 C;
};

// Spherical / ball-and-socket: point lock only
struct BallJoint : public JointBase
{
    BallJointConfig FConfig;

    BallJointState FState; // TODO MOVE THIS OUT IN A SEPARATE SOLVER

    vec3 FPointImpulse; // warm-start λx,λy,λz

    void Initialize(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const BallJointConfig& Config);
    void Destroy();
    void PreSolve(Island* parIsland, const float parDT) override;
    void Solve(Island* parIsland, const float parDT) override;
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

    void PreSolve(Island* parIsland, const float parDT) override { }
    void Solve(Island* parIsland, const float parDT) override { }
};

} // namespace Physics
} // namespace ECSEngine