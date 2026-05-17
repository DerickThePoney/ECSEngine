#pragma once
#include "Common/PoolAllocator.h"

namespace ECSEngine
{
namespace Physics
{
struct ContactPointState
{
    vec3 CtoA; // Contact to A COM
    vec3 CtoB; // Contact to B COM
    float Penetration = 0.f;
    float NormalImpulse = 0.f; // Accumulated normal impulse (for warm starting)
    float TangentImpulses[2]; // Accumulated tangent impulse (for warm starting)
    float Bias = 0.f;
    float RestitutionBias = 0.f;
    float NormalMass = 0.f;
    float TangentMass[2];
};

struct ContactState
{
    DECLARE_POOL_ALLOCATED(ContactState);

public:
    ContactPointState ContactPoints[8];
    i32 NumberOfContacts = 0;
    vec3 TangentVectors[2]; // Tangent vectors
    vec3 Normal; // From A to B
    vec3 CenterA;
    vec3 CenterB;
    mat3 IA;
    mat3 IB;
    float MA;
    float MB;
    float Restitution;
    float Friction;
    i32 IndexA;
    i32 IndexB;
};
} // namespace Physics
} // namespace ECSEngine