#pragma once

#include "Common/PoolAllocator.h"
#include "IJointBatch.h"

namespace ECSEngine
{
namespace Physics
{

// Per-island scratch for one weld. Point fields match the ball joint.
// Angular fields lock the three rotational degrees of freedom, with no cone and no motor.
struct WeldJointState
{
    DECLARE_POOL_ALLOCATED(WeldJointState);

public:
    WeldJoint* Joint = nullptr;

    i32 IndexA = -1;
    i32 IndexB = -1;
    float MA = 0.f;
    float MB = 0.f;
    mat3 IA;
    mat3 IB;

    vec3 xA, xB; // world anchors
    vec3 rA, rB; // anchor relative to each center of mass
    vec3 C; // xB - xA, world
    vec3 M_eff; // effective mass per world axis
    vec3 bias; // Baumgarte bias per world axis

    vec3 e0, e1, e2; // body A axes in world; the angular constraints are solved along these
    vec3 angularC; // rotation-vector error, components along e0, e1, e2
    vec3 angularM_eff;
    vec3 angularBias;

    vec3 PointImpulse; // accumulated, carried across frames
    vec3 AngularImpulse;
};

class WeldJointBatch : public JointBatch<WeldJoint, WeldJointState, EJointType::WELD>
{
public:
    void Initialize(Island* island) override;
    void PreSolve(Island* island, float dt) override;
    void Solve(Island* island) override;
    void Shutdown() override;
};

} // namespace Physics
} // namespace ECSEngine
