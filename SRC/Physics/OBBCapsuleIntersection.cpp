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
    constexpr u32 MaxCandidates = 14; // 12 arêtes + 2 endpoints
    CandidateContact Candidates[MaxCandidates];
    u32 NumCandidates = 0;

    for (const auto& Edge : Edges)
    {
        float tA, tB;
        vec3 OnSeg, OnEdge;
        const float DistSq = GeometryHelpers::ClosestPointSegmentSegment(ASLocal, AELocal, tA, OnSeg, Edge[0], Edge[1], tB, OnEdge);

        Candidates[NumCandidates++] = { OnSeg, OnEdge, DistSq };
    }

    const vec3 Endpoints[2] = { ASLocal, AELocal };
    for (const vec3& Endpoint : Endpoints)
    {
        const bool IsInside = (Endpoint.x >= MinB.x && Endpoint.x <= MaxB.x && Endpoint.y >= MinB.y && Endpoint.y <= MaxB.y && Endpoint.z >= MinB.z && Endpoint.z <= MaxB.z);

        if (IsInside)
        {
            // Utiliser la direction depuis le centre de l'AABB vers l'endpoint
            // pour déterminer la face de sortie naturelle
            const vec3 CenterToEndpoint = Endpoint - CenterBLocal;

            // Normaliser par les demi-extents pour trouver la face dominante
            const vec3 Normalized = vec3(CenterToEndpoint.x / HalfExtents.x, CenterToEndpoint.y / HalfExtents.y, CenterToEndpoint.z / HalfExtents.z);

            // La composante dominante donne la face de sortie
            const vec3 Abs = vec3(fabsf(Normalized.x), fabsf(Normalized.y), fabsf(Normalized.z));

            vec3 SurfacePoint = Endpoint;
            if (Abs.x >= Abs.y && Abs.x >= Abs.z)
                SurfacePoint.x = Normalized.x > 0.f ? MaxB.x : MinB.x;
            else if (Abs.y >= Abs.x && Abs.y >= Abs.z)
                SurfacePoint.y = Normalized.y > 0.f ? MaxB.y : MinB.y;
            else
                SurfacePoint.z = Normalized.z > 0.f ? MaxB.z : MinB.z;

            const float PenetrationDist = LengthSq(Endpoint - SurfacePoint);
            Candidates[NumCandidates++] = { Endpoint, SurfacePoint, -(PenetrationDist) };
        }
        else
        {
            const vec3 Clamped = Clamp(Endpoint, MinB, MaxB);
            const float DistSq = LengthSq(Endpoint - Clamped);
            Candidates[NumCandidates++] = { Endpoint, Clamped, DistSq };
        }
    }

    // Trouver la normale de référence depuis le meilleur candidat
    u32 BestIdx = 0;
    forrange(i, 0, NumCandidates)
    {
        const float Di = Candidates[i].DistSq;
        const float Db = Candidates[BestIdx].DistSq;
        const bool IEndpoint = (i >= 12);
        const bool BEndpoint = (BestIdx >= 12);
        const float Bias = (!BEndpoint && IEndpoint) ? 1e-4f : 0.f;

        if (Di < Db + Bias)
            BestIdx = i;
    }

    const CandidateContact& Best = Candidates[BestIdx];
    const bool BestIsInside = (BestIdx >= 12) && (Best.DistSq < 0.f);

    const vec3 BestDiffLocal = Best.OnSegLocal - Best.OnBoxLocal;
    const float BestDistSqLocal = LengthSq(BestDiffLocal);

    // Test de rejet :
    // - Cas extérieur : distance segment->surface doit être <= rayon
    // - Cas intérieur : toujours en contact par définition
    if (!BestIsInside && BestDistSqLocal > parRadiusA * parRadiusA)
        return false;

    const float BestDistLocal = sqrtf(BestDistSqLocal);
    C->FContactNormal = (BestDistSqLocal > 1e-6f) ? (parTransformB * vec4::MakeHomogeneousDirectionVec4(BestDiffLocal) / BestDistLocal).xyz() : vec3(0.f, 1.f, 0.f);
    vec3 OriginalContactNormal = C->FContactNormal;

    if (parInvertResult)
        C->FContactNormal = Invert(OriginalContactNormal);

    // Ajouter tous les contacts dont la normale est compatible et la distance dans le rayon
    for (const auto& Candidate : Candidates)
    {
        const bool CandidateIsInside = (Candidate.DistSq < 0.f);
        const vec3 DiffLocal = Candidate.OnSegLocal - Candidate.OnBoxLocal;
        const float DistSqLocal = LengthSq(DiffLocal);

        // Même logique : cas intérieur toujours valide, cas extérieur filtré par rayon
        if (!CandidateIsInside && DistSqLocal > parRadiusA * parRadiusA)
            continue;

        // Filtrer les contacts dont la normale est opposée (évite les doublons parasites)
        const vec3 CandidateNormalLocal = (DistSqLocal > 1e-6f) ? DiffLocal / sqrtf(DistSqLocal) : BestDiffLocal / BestDistLocal;

        if (Dot(CandidateNormalLocal, BestDiffLocal) < 0.f)
            continue;

        const float DistLocal = sqrtf(DistSqLocal);
        const vec3 OnSeg = (parTransformB * vec4(Candidate.OnSegLocal, 1.f)).xyz();
        const vec3 OnBox = (parTransformB * vec4(Candidate.OnBoxLocal, 1.f)).xyz();

        ContactPoint CP;
        const vec3 ContactOnCapsule = OnSeg - OriginalContactNormal * parRadiusA;
        CP.FPosition = parInvertResult ? ContactOnCapsule : OnBox;
        CP.FPenetration = parRadiusA - DistLocal;
        C->FManifold.FContactPoints.push_back(CP);
    }

    return !C->FManifold.FContactPoints.empty();
}
} // namespace Physics
} // namespace ECSEngine