#include "stdafx.h"

#include "CapsuleSphereIntersection.h"

#include "Contact.h"
#include "GeometryHelpers.h"

namespace ECSEngine
{
namespace Physics
{
bool CapsuleSphereIntersection(Contact* C,
      const mat4& parTransformA,
      const vec3 parCenterA,
      const float parRadiusA,
      const float parHalfLengthA,
      const mat4& parTransformB,
      const vec3 parCenterB,
      const float parRadiusB,
      const bool parInvertResult)
{
    // Transformer les centres en world space
    const vec3 CenterAW = (parTransformA * vec4::MakeHomogeneousPositionVec4(parCenterA)).xyz();
    const vec3 CenterBW = (parTransformB * vec4::MakeHomogeneousPositionVec4(parCenterB)).xyz();

    // Axe de la capsule (colonne Y de la matrice de transform)
    const vec3 AxisA = parTransformA.Column(1).xyz();

    // Endpoints du segment de la capsule en world space
    const vec3 AS = CenterAW - AxisA * parHalfLengthA;
    const vec3 AE = CenterAW + AxisA * parHalfLengthA;

    // Closest point sur le segment par rapport au centre de la sphère
    // La sphère est traitée comme un segment dégénéré (start == end)
    float tA, tB;
    vec3 closestA, closestB;
    const float distSq = GeometryHelpers::ClosestPointSegmentSegment(AS, AE, tA, closestA, CenterBW, CenterBW, tB, closestB);

    const float touchDist = parRadiusA + parRadiusB;
    const float touchDistSq = touchDist * touchDist;

    if (distSq > touchDistSq)
        return false;

    const float dist = sqrtf(distSq);
    C->FContactNormal = (distSq > 1e-6f) ? (closestB - closestA) / dist : vec3(0.f, 1.f, 0.f);

    if (parInvertResult)
    {
        C->FContactNormal = Invert(C->FContactNormal);
    }

    ContactPoint CP;
    CP.FPosition = (parInvertResult) ? closestB + C->FContactNormal * parRadiusB : closestA + C->FContactNormal * parRadiusA;
    CP.FPenetration = touchDist - dist;
    C->FManifold.FContactPoints.push_back(CP);
    return true;
}
} // namespace Physics
} // namespace ECSEngine