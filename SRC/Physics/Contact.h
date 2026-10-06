#pragma once
#include "Common/BitSet.h"
#include "Common/PoolAllocator.h"
#include "PhysicsBodyHandle.h"

namespace ECSEngine
{
namespace Physics
{
struct Contact;
struct ContactEdge
{
    PhysicsBodyHandle FOtherBody;

    Contact* FContact = nullptr;

    ContactEdge* FPrev = nullptr;
    ContactEdge* FNext = nullptr;
};

enum EContactFlag : u32
{
    CT_TOUCHING,
    CT_WAS_TOUCHING,
    CT_ISLAND,
    CT_COUNT
};

// in stands for "incoming"
// out stands for "outgoing"
// I stands for "incident"
// R stands for "reference"
// See D. Gregorius GDC 2015 on creating contacts for more details
// Each feature pair is used to cache solutions from one physics tick to another. This is
// called warmstarting, and lets boxes stack and stay stable. Feature pairs identify points
// of contact over multiple physics ticks. Each feature pair is the junction of an incoming
// feature and an outgoing feature, usually a result of clipping routines. The exact info
// stored in the feature pair can be arbitrary as long as the result is a unique ID for a
// given intersecting configuration.
union FeaturePair
{
    struct
    {
        u8 inR;
        u8 outR;
        u8 inI;
        u8 outI;
        u8 ShapeAIndex;
        u8 ShapeBIndex;
    };

    i64 key = 0;
};

struct ContactPoint
{
    DECLARE_POOL_ALLOCATED(ContactPoint);

public:
    vec3 FPosition;
    float FPenetration = 0.f;
    float FNormalImpulse = 0.f;
    float FTangentImpulse[2] = { 0.f, 0.f };
    FeaturePair FP;
};

struct ContactManifold
{
    std::vector<ContactPoint> FContactPoints;

    void clear() { FContactPoints.clear(); }
};

struct Contact
{
    DECLARE_POOL_ALLOCATED(Contact);

public:
    void Evaluate();

private:
    void ComputeBasis();

public:
    PhysicsBodyHandle FFirstBody;
    PhysicsBodyHandle FSecondBody;

    // Contact data
    ContactEdge FFirstBodyEdge;
    ContactEdge FSecondBodyEdge;

    // FreeList implementation
    Contact* FNext = nullptr;
    Contact* FPrev = nullptr;

    // Manifold, normal and tangents
    ContactManifold FManifold;
    vec3 FContactNormal;
    vec3 FContactTangents[2];

    // Friction and restitution
    float FFriction = 0.f;
    float FRestitution = 0.f;

    // Additional computation stuff // PER CONTACT POINT
    float FBias = 0.f;

    BitSet<EContactFlag::CT_COUNT> FFlags;

#ifdef PERFORM_SECURITY_CHECKS
    vec3 SeparatingAxis;
    vec3 SeparationVector;
#endif
};
} // namespace Physics
} // namespace ECSEngine