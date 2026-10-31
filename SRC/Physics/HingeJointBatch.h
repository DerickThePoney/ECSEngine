#pragma once

#include "Common/PoolAllocator.h"
#include "IJointBatch.h"

namespace ECSEngine
{
namespace Physics
{

struct HingeJointState
{
    DECLARE_POOL_ALLOCATED(HingeJointState);

public:
    HingeJoint* Joint = nullptr;

    i32 IndexA = -1;
    i32 IndexB = -1;
    float MA = 0.f;
    float MB = 0.f;
    mat3 IA;
    mat3 IB;

    vec3 xA, xB;
    vec3 rA, rB;
    vec3 C;
    vec3 M_eff;
    vec3 bias;

    vec3 aA;
    vec3 aB;
    vec3 t1, t2;
    vec3 u1, u2;
    float axisM_eff[2];
    float axisBias[2];
    float axisC[2];

    float motorM_eff = 0.f;
    float motorSpeed = 0.f;
    float maxMotorImpulse = 0.f;

    float theta = 0.f;
    float limitM_eff = 0.f;
    float limitBias = 0.f;
    EHingeLimitState LimitState = EHingeLimitState::Inactive;
    float PrevAngle = 0.f;

    vec3 PointImpulse;
    float AxisImpulse[2] = { };
    float LimitImpulse = 0.f;
    float MotorImpulse = 0.f;
};

class HingeJointBatch : public JointBatch<HingeJoint, HingeJointState, EJointType::HINGE>
{
public:
    void Initialize(Island* island) override;
    void PreSolve(Island* island, float dt) override;
    void Solve(Island* island) override;
    void Shutdown() override;
};

} // namespace Physics
} // namespace ECSEngine
