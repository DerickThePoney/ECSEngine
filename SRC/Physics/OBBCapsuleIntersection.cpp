#include "stdafx.h"

#include "OBBCapsuleIntersection.h"

#include "Contact.h"
#include "GeometryHelpers.h"

namespace ECSEngine
{
namespace Physics
{
bool OBBCapsuleIntersection(Contact* C,
      const mat4& parTransformA,
      const vec3& parCenterA,
      const float parRadiusA,
      const float parHalfLengthA,
      const mat4& parTransformB,
      const AABB3f& parBoundingBoxB,
      const bool parInvertResult)
{
    // Axe de la capsule (colonne Y de la matrice de transform)
    const vec3 AxisA = parTransformA.Column(1).xyz();

    // Endpoints du segment de la capsule en world space
    const vec3 CenterAW = (parTransformA * vec4::MakeHomogeneousPositionVec4(parCenterA)).xyz();
    const vec3 AS = CenterAW - AxisA * parHalfLengthA;
    const vec3 AE = CenterAW + AxisA * parHalfLengthA;

    // Passer les endpoints en espace local de l'OBB
    // En espace local, le problème devient capsule vs AABB
    const mat4 InvTransformB = Invert(parTransformB);
    const vec3 ASLocal = (InvTransformB * vec4::MakeHomogeneousPositionVec4(AS)).xyz();
    const vec3 AELocal = (InvTransformB * vec4::MakeHomogeneousPositionVec4(AE)).xyz();

    const vec3 CenterBLocal = parBoundingBoxB.Center();
    const vec3 HalfExtents = parBoundingBoxB.HalfExtent();
    const vec3 MinB = CenterBLocal - HalfExtents;
    const vec3 MaxB = CenterBLocal + HalfExtents;

    // Les 12 arêtes de l'AABB, groupées par axe
    const vec3 Edges[12][2] = {
        // Parallèles à X
        { vec3(MinB.x, MinB.y, MinB.z), vec3(MaxB.x, MinB.y, MinB.z) },
        { vec3(MinB.x, MaxB.y, MinB.z), vec3(MaxB.x, MaxB.y, MinB.z) },
        { vec3(MinB.x, MinB.y, MaxB.z), vec3(MaxB.x, MinB.y, MaxB.z) },
        { vec3(MinB.x, MaxB.y, MaxB.z), vec3(MaxB.x, MaxB.y, MaxB.z) },
        // Parallèles à Y
        { vec3(MinB.x, MinB.y, MinB.z), vec3(MinB.x, MaxB.y, MinB.z) },
        { vec3(MaxB.x, MinB.y, MinB.z), vec3(MaxB.x, MaxB.y, MinB.z) },
        { vec3(MinB.x, MinB.y, MaxB.z), vec3(MinB.x, MaxB.y, MaxB.z) },
        { vec3(MaxB.x, MinB.y, MaxB.z), vec3(MaxB.x, MaxB.y, MaxB.z) },
        // Parallèles à Z
        { vec3(MinB.x, MinB.y, MinB.z), vec3(MinB.x, MinB.y, MaxB.z) },
        { vec3(MaxB.x, MinB.y, MinB.z), vec3(MaxB.x, MinB.y, MaxB.z) },
        { vec3(MinB.x, MaxB.y, MinB.z), vec3(MinB.x, MaxB.y, MaxB.z) },
        { vec3(MaxB.x, MaxB.y, MinB.z), vec3(MaxB.x, MaxB.y, MaxB.z) },
    };

    // Au lieu de BestDistSq/BestOnSegLocal/BestOnBoxLocal uniques,
    // collecter tous les contacts sous le seuil

    struct CandidateContact
    {
        vec3 OnSegLocal;
        vec3 OnBoxLocal;
        float DistSq;
    };

    // Remplacer les deux boucles par une collecte de candidats
    std::vector<CandidateContact> Candidates;

    for (const auto& Edge : Edges)
    {
        float tA, tB;
        vec3 OnSeg, OnEdge;
        const float DistSq = GeometryHelpers::ClosestPointSegmentSegment(ASLocal, AELocal, tA, OnSeg, Edge[0], Edge[1], tB, OnEdge);

        Candidates.push_back({ OnSeg, OnEdge, DistSq });
    }

    const vec3 Endpoints[2] = { ASLocal, AELocal };
    for (const vec3& Endpoint : Endpoints)
    {
        const bool IsInside = (Endpoint.x >= MinB.x && Endpoint.x <= MaxB.x && Endpoint.y >= MinB.y && Endpoint.y <= MaxB.y && Endpoint.z >= MinB.z && Endpoint.z <= MaxB.z);

        if (IsInside)
        {
            const float Distances[6] = {
                Endpoint.x - MinB.x,
                MaxB.x - Endpoint.x,
                Endpoint.y - MinB.y,
                MaxB.y - Endpoint.y,
                Endpoint.z - MinB.z,
                MaxB.z - Endpoint.z,
            };

            int BestFace = 0;
            for (int i = 1; i < 6; ++i)
                if (Distances[i] < Distances[BestFace])
                    BestFace = i;

            vec3 SurfacePoint = Endpoint;
            switch (BestFace)
            {
            case 0:
                SurfacePoint.x = MinB.x;
                break;
            case 1:
                SurfacePoint.x = MaxB.x;
                break;
            case 2:
                SurfacePoint.y = MinB.y;
                break;
            case 3:
                SurfacePoint.y = MaxB.y;
                break;
            case 4:
                SurfacePoint.z = MinB.z;
                break;
            case 5:
                SurfacePoint.z = MaxB.z;
                break;
            }

            const float PenetrationDist = Distances[BestFace];
            Candidates.push_back({ Endpoint, SurfacePoint, -(PenetrationDist * PenetrationDist) });
        }
        else
        {
            const vec3 Clamped = Clamp(Endpoint, MinB, MaxB);
            const float DistSq = LengthSq(Endpoint - Clamped);
            Candidates.push_back({ Endpoint, Clamped, DistSq });
        }
    }

    // Trouver la normale de référence depuis le meilleur candidat
    const auto& Best = *std::min_element(Candidates.begin(), Candidates.end(), [](const CandidateContact& A, const CandidateContact& B) { return A.DistSq < B.DistSq; });

    const vec3 BestDiffLocal = Best.OnSegLocal - Best.OnBoxLocal;
    const float BestDistSqLocal = LengthSq(BestDiffLocal);

    if (BestDistSqLocal > parRadiusA * parRadiusA)
        return false;

    const float BestDistLocal = sqrtf(BestDistSqLocal);
    vec3 ContactNormal = (BestDistSqLocal > 1e-6f) ? (parTransformB * vec4::MakeHomogeneousDirectionVec4(BestDiffLocal) / BestDistLocal).xyz() : vec3(0.f, 1.f, 0.f);

    if (parInvertResult)
        ContactNormal = Invert(ContactNormal);

    C->FContactNormal = ContactNormal;

    // Ajouter tous les contacts dont la normale est compatible et la distance dans le rayon
    for (const auto& Candidate : Candidates)
    {
        const vec3 DiffLocal = Candidate.OnSegLocal - Candidate.OnBoxLocal;
        const float DistSqLocal = LengthSq(DiffLocal);

        if (DistSqLocal > parRadiusA * parRadiusA)
            continue;

        // Filtrer les contacts dont la normale est opposée (évite les doublons parasites)
        const vec3 CandidateNormalLocal = (DistSqLocal > 1e-6f) ? DiffLocal / sqrtf(DistSqLocal) : BestDiffLocal / BestDistLocal;

        if (Dot(CandidateNormalLocal, BestDiffLocal) < 0.f)
            continue;

        const float DistLocal = sqrtf(DistSqLocal);
        const vec3 OnSeg = (parTransformB * vec4(Candidate.OnSegLocal, 1.f)).xyz();
        const vec3 OnBox = (parTransformB * vec4(Candidate.OnBoxLocal, 1.f)).xyz();

        ContactPoint CP;
        const vec3 ContactOnCapsule = OnSeg - ContactNormal * parRadiusA;
        CP.FPosition = parInvertResult ? ContactOnCapsule : OnBox;
        CP.FPenetration = parRadiusA - DistLocal;
        C->FManifold.FContactPoints.push_back(CP);
    }

    return !C->FManifold.FContactPoints.empty();
}
} // namespace Physics
} // namespace ECSEngine