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
    void Initialize(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB);
    virtual void Destroy();

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
    void PreSolve(Island* parIsland, const float parDT) override;
    void Solve(Island* parIsland, const float parDT) override;
};

struct HingeJointState
{
    // Point-to-point (same as BallJointState)
    vec3 xA, xB;
    vec3 rA, rB;
    vec3 C; // xB - xA
    vec3 M_eff; // per world axis
    vec3 bias;

    // Axis alignment
    vec3 aA; // world hinge axis
    vec3 aB;
    vec3 t1, t2; // ⊥ aA
    vec3 u1, u2; // angular impulse axes (e.g. aB × t_k)
    float axisM_eff[2];
    float axisBias[2];
    float axisC[2]; // t1·aB, t2·aB

    // Limit (optional)
    float theta = 0.f;
    float limitM_eff = 0.f;
    float limitBias = 0.f;
    // active limit state can stay on the joint (FLimitState)
};

enum class EHingeLimitState : u8
{
    Inactive,
    AtLower,
    AtUpper
};

// Hinge = ball point lock + axis freeze + optional limit
struct HingeJoint : public JointBase
{
    HingeJointConfig FConfig;
    HingeJointState FState;

    // Bind-time (filled in Initialize, not in editor)
    vec3 FAxisALocal; // copy/normalized from config
    vec3 FAxisBLocal; // world axisA expressed in B at bind
    vec3 FRefALocal; // unit ⊥ axis on A
    vec3 FRefBLocal; // same world ref expressed in B at bind
    float FReferenceAngle = 0.f; // usually 0 = limits relative to bind pose
    float FPrevAngle = 0.f; // for unwrap

    // Warm-start λ (survive across steps)
    vec3 FPointImpulse; // ball point lock
    float FAxisImpulse[2] = { };
    float FLimitImpulse = 0.f;
    EHingeLimitState FLimitState = EHingeLimitState::Inactive;

    // Motor Data
    bool FEnableMotor = false;
    float FMotorSpeed = 0.f;
    float FMaxMotorTorque = 0.f;

    bool FUseTargetAngle = false;
    float FTargetAngle = 0.f;
    float FMotorStiffness = 0.f;

    float FMotorImpulse = 0.f;

    void Initialize(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const HingeJointConfig& Config);
    void PreSolve(Island* parIsland, const float parDT) override;
    void Solve(Island* parIsland, const float parDT) override;
};

} // namespace Physics
} // namespace ECSEngine