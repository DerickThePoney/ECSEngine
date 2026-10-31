#pragma once

#include "Common/PoolAllocator.h"
#include "IJointBatch.h"

namespace ECSEngine
{
namespace Physics
{

struct BallJointState
{
    DECLARE_POOL_ALLOCATED(BallJointState);

public:
    BallJoint* Joint = nullptr;

    i32 IndexA = -1;
    i32 IndexB = -1;
    float MA = 0.f;
    float MB = 0.f;
    mat3 IA;
    mat3 IB;

    vec3 xA, xB;
    vec3 rA, rB;
    vec3 M_eff;
    vec3 bias;
    vec3 C;

    vec3 e0, e1, e2;
    vec3 motorM_eff;
    vec3 motorVelocityWorld;
    float maxMotorImpulse = 0.f;

    bool coneLimitActive = false;
    vec3 coneAxis;
    float coneAngle = 0.f;
    float coneM_eff = 0.f;
    float coneBias = 0.f;

    vec3 PointImpulse;
    vec3 MotorImpulse;
    float ConeLimitImpulse = 0.f;
};

class BallJointBatch : public JointBatch<BallJoint, BallJointState, EJointType::BALL>
{
public:
    void Initialize(Island* island) override;
    void PreSolve(Island* island, float dt) override;
    void Solve(Island* island) override;
    void Shutdown() override;
};

} // namespace Physics
} // namespace ECSEngine
