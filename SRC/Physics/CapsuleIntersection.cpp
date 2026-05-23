#include "stdafx.h"

#include "CapsuleIntersection.h"

#include "Contact.h"
#include "GeometryHelpers.h"

bool ECSEngine::Physics::CapsuleIntersection(Contact* C,
      const mat4& parTransformA,
      const vec3 parCenterA,
      const float parRadiusA,
      const float parHalfLengthA,
      const mat4& parTransformB,
      const vec3 parCenterB,
      const float parRadiusB,
      const float parHalfLengthB)
{
    // Transformer les centres en world space
    const vec3 CenterAW = (parTransformA * vec4::MakeHomogeneousPositionVec4(parCenterA)).xyz();
    const vec3 CenterBW = (parTransformB * vec4::MakeHomogeneousPositionVec4(parCenterB)).xyz();

    // Axe de chaque capsule (colonne Y de la matrice de transform)
    const vec3 AxisA = parTransformA.Column(1).xyz();
    const vec3 AxisB = parTransformB.Column(1).xyz();

    // Endpoints des segments en world space
    const vec3 AS = CenterAW - AxisA * parHalfLengthA;
    const vec3 AE = CenterAW + AxisA * parHalfLengthA;
    const vec3 BS = CenterBW - AxisB * parHalfLengthB;
    const vec3 BE = CenterBW + AxisB * parHalfLengthB;

    // Closest points entre les deux segments
    float tA, tB;
    vec3 closestA, closestB;
    const float distSq = GeometryHelpers::ClosestPointSegmentSegment(AS, AE, tA, closestA, BS, BE, tB, closestB);

    const float touchDist = parRadiusA + parRadiusB;
    const float touchDistSq = touchDist * touchDist;

    if (distSq > touchDistSq)
        return false;

    const float dist = sqrtf(distSq);
    C->FContactNormal = (distSq > 1e-6f) ? (closestB - closestA) / dist : vec3(1.f, 0.f, 0.f);

    ContactPoint CP;
    CP.FPosition = closestA + C->FContactNormal * parRadiusA;
    CP.FPenetration = touchDist - dist;
    C->FManifold.FContactPoints.push_back(CP);
    return true;
}
