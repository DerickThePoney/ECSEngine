#pragma once

#include "Common/PoolAllocator.h"
#include "IJointBatch.h"

namespace ECSEngine
{
namespace Physics
{

struct DistanceJointState
{
    DECLARE_POOL_ALLOCATED(DistanceJointState);

public:
    DistanceJoint* Joint = nullptr;

    i32 IndexA = -1;
    i32 IndexB = -1;
    float MA = 0.f;
    float MB = 0.f;
    mat3 IA;
    mat3 IB;

    vec3 xA, xB;
    vec3 rA, rB;
    vec3 d, n;
    float C = 0.f;
    float M_eff = 0.f;
    float bias = 0.f;
    float Impulse = 0.f;
};

class DistanceJointBatch : public JointBatch<DistanceJoint, DistanceJointState, EJointType::DISTANCE>
{
public:
    void Initialize(Island* island) override;
    void PreSolve(Island* island, float dt) override;
    void Solve(Island* island) override;
    void Shutdown() override;
};

} // namespace Physics
} // namespace ECSEngine
