#include "stdafx.h"

#include "PolygonTriangulator.h"

#include "IntersectionRoutines.h"
#include "Polygon.h"
#include "Triangle.h"

namespace ECSEngine
{

namespace
{
u32 PreviousIndex(const u32 idx, const u32 size)
{
    return (idx == 0) ? (size - 1) : (idx - 1);
}
u32 NextIndex(const u32 idx, const u32 size)
{
    return (idx == size - 1) ? 0 : (idx + 1);
}
bool EarTest(const u32 i_m1, const u32 i, const u32 i_1, const Polygon2D& parPolygon)
{
    u32 idx = NextIndex(i_1, (u32)parPolygon.size());
    Triangle2D currentTri(parPolygon[i_m1], parPolygon[i], parPolygon[i_1]);
    while (idx != i_m1)
    {
        bool thisRes = Intersection::PointInTriangle2D(currentTri, parPolygon[idx], true);
        if (thisRes)
            return false;

        idx = NextIndex(idx, (u32)parPolygon.size());
    }

    return true;
}

bool IsReflex(const u32 i_m1, const u32 i, const u32 i_1, const Polygon2D& parPolygon)
{
    glm::vec2 p1 = parPolygon[i_m1];
    glm::vec2 p2 = parPolygon[i];
    glm::vec2 p3 = parPolygon[i_1];
    float tmp = (p3.y - p1.y) * (p2.x - p1.x) - (p3.x - p1.x) * (p2.y - p1.y);
    return tmp < 0;
}

void UpdateAdjacentVertex(const u32 idx, std::vector<u32>& convex, std::vector<u32>& reflex, std::vector<u32>& ears, const Polygon2D& parPolygon)
{
    auto itConvex = std::find(convex.begin(), convex.end(), idx);
    const u32 i_m1 = PreviousIndex(idx, (u32)parPolygon.size());
    const u32 i_p1 = NextIndex(idx, (u32)parPolygon.size());
    if (itConvex != convex.end())
    {
        // do an ear test
        auto itEar = std::find(ears.begin(), ears.end(), idx);
        if (EarTest(i_m1, idx, i_p1, parPolygon))
        {
            if (itEar == ears.end())
                ears.push_back(idx);
        }
        else
        {
            if (itEar != ears.end())
                ears.erase(itEar);
        }
    }
    else
    {
        auto itReflex = std::find(reflex.begin(), reflex.end(), idx);
        AlwaysCheckedAssert(itReflex != reflex.end());

        bool isReflex = IsReflex(i_m1, idx, i_p1, parPolygon);

        if (!isReflex)
        {
            // on devient convex, ear test + lists update
            reflex.erase(itReflex);
            convex.push_back(idx);

            if (EarTest(i_m1, idx, i_p1, parPolygon))
                ears.push_back(idx);
        }
    }
}

void InsertHoleIntoPolygon(Polygon2D& parPolygon, const Polygon2D& parHole)
{
    if (parHole.empty())
        return;

    // Check polygon orientation
    Polygon2D holeToUse = parHole;
    if (!holeToUse.IsClockWise())
        holeToUse = holeToUse.Revert();

    // TODO Check inclusion in polygon ?

    // Find mutually visible edges
    // 1- Get the hole vertex M with max X coordinate
    const std::vector<glm::vec2>& holeVertices = holeToUse.data();
    u32 maxXVertex = 0;
    float maxXCoord = holeVertices[0].x;

    forrange(i, 1, holeVertices.size())
    {
        if (holeVertices[i].x > maxXCoord)
        {
            maxXCoord = holeVertices[i].x;
            maxXVertex = (u32)i;
        }
    }

    // 2 - Intersect Ray(M, (1,0)) with the edges of the polygon
    Ray2D ray = Ray2D(holeVertices[maxXVertex], glm::vec2(1.f, 0.f));
    Intersection::LinearComponentIntersection intersectionResult;
    u32 closestPolygonEdgeIndex = -1;
    const bool intersect = Intersection::RayPolygonClosestIntersection2D(ray, parPolygon, true, intersectionResult, closestPolygonEdgeIndex);
    AlwaysCheckedAssert(intersect);
    if (!intersect)
        return;

    AssertRelease(closestPolygonEdgeIndex != -1);

    u32 PIndex = 0;
    // 3 - If intersection I is vertex (0, 1 on Intersection2) -> terminate
    if (intersectionResult.Intersection2 == 0.f)
    {
        PIndex = closestPolygonEdgeIndex;
    }
    else if (intersectionResult.Intersection2 == 1.f)
    {
        PIndex = NextIndex(closestPolygonEdgeIndex, (u32)parPolygon.size());
    }
    else
    {
        // 4 - If intersection I is on the edge, select P the endpoint of max X coord on the edge
        const u32 nextIndex = NextIndex(closestPolygonEdgeIndex, (u32)parPolygon.size());
        PIndex = (parPolygon[closestPolygonEdgeIndex].x > parPolygon[nextIndex].x) ? closestPolygonEdgeIndex : nextIndex;

        // 5 - Search all polygon reflex vertices for those in triangle MIP, excluding P. If none, M and P are mutually visible, terminate.
        Triangle2D mipTriangle = { holeVertices[maxXVertex], holeVertices[maxXVertex] + intersectionResult.Intersection1 * glm::vec2(1.f, 0.f), parPolygon[PIndex] };
        std::vector<u32> reflexVerticesInMIP;
        reflexVerticesInMIP.reserve(parPolygon.size());
        forrange(idx, 0, parPolygon.size())
        {
            if (idx == PIndex)
                continue;
            const u32 i = (u32)idx;
            const u32 i_m1 = PreviousIndex(i, (u32)parPolygon.size());
            const u32 i_p1 = NextIndex(i, (u32)parPolygon.size());
            const bool reflex = IsReflex(i_m1, i, i_p1, parPolygon);
            if (reflex)
            {
                if (Intersection::PointInTriangle2D(mipTriangle, parPolygon[i]))
                {
                    reflexVerticesInMIP.push_back(i);
                }
            }
        }
        // 6 - Choose the reflex vertex R in MIP minimizing the angle between (1,0) and MR - > terminate
        if (reflexVerticesInMIP.size() > 1)
        {
            float maxDot = -1.f;
            u32 bestReflexVertex = -1;
            foreachitemconst(reflexVertex, reflexVerticesInMIP)
            {
                Segment2D mr = Segment2D(holeVertices[maxXVertex], parPolygon[reflexVertex]);
                const float dotRes = glm::dot(glm::vec2(1.f, 0.f), mr.DirectionNormalized());
                if (dotRes > maxDot)
                {
                    maxDot = dotRes;
                    bestReflexVertex = reflexVertex;
                }
            }

            AssertRelease(bestReflexVertex != -1);
            PIndex = bestReflexVertex;
        }
        else if (reflexVerticesInMIP.size() == 1)
        {
            PIndex = reflexVerticesInMIP[0];
        }
    }

    // insert the hole into the polygon at the right place
    std::vector<glm::vec2> newPoints;
    newPoints.reserve(parPolygon.size() + parHole.size() + 2);

    // on rempli jusqu'a PIndex
    u32 currentPolygonIndex = 0;
    while (currentPolygonIndex != PIndex)
    {
        newPoints.push_back(parPolygon[currentPolygonIndex]);
        currentPolygonIndex = NextIndex(currentPolygonIndex, (u32)parPolygon.size());
    }
    newPoints.push_back(parPolygon[currentPolygonIndex]);

    // On mets le hole en commencant par maxXIndex, que l'on mets deux fois
    u32 currentHoleIndex = maxXVertex;
    do
    {
        newPoints.push_back(holeToUse[currentHoleIndex]);
        currentHoleIndex = NextIndex(currentHoleIndex, (u32)holeToUse.size());
    } while (currentHoleIndex != maxXVertex);
    newPoints.push_back(holeToUse[currentHoleIndex]);

    // on continue le polygone principal en recommançant par PIndex
    forrange(i, PIndex, parPolygon.size()) { newPoints.push_back(parPolygon[i]); }

    parPolygon.set_points(std::move(newPoints));
}

} // namespace

std::vector<Triangle2D> PolygonTriangulator::Triangulate(const Polygon2D& parPolygon)
{
    std::vector<Triangle2D> triangles;
    if (parPolygon.size() == 3)
    {
        triangles.push_back(Triangle2D(parPolygon[0], parPolygon[1], parPolygon[2]));
        return triangles;
    }

    const bool isClockwise = parPolygon.IsClockWise();
    Polygon2D touse;
    if (isClockwise)
    {
        touse = parPolygon.Revert();
    }
    else
    {
        touse = parPolygon;
    }

    std::vector<u32> ears;
    std::vector<u32> reflex;
    std::vector<u32> convex;
    forrange(idx, 0, touse.size())
    {
        const u32 i = (u32)idx;
        const u32 i_m1 = PreviousIndex(i, (u32)touse.size());
        const u32 i_p1 = NextIndex(i, (u32)touse.size());
        const bool isReflex = IsReflex(i_m1, i, i_p1, parPolygon);
        if (isReflex)
        {
            reflex.push_back(i);
        }
        else
        {
            convex.push_back(i);
            if (EarTest(i_m1, i, i_p1, touse))
                ears.push_back(i);
        }
    }

    while (!ears.empty())
    {
        const u32 currentEar = ears[0];
        u32 currentEar_m1 = PreviousIndex(currentEar, (u32)touse.size());
        u32 currentEar_1 = NextIndex(currentEar, (u32)touse.size());

        ears.erase(ears.begin());

        triangles.push_back(Triangle2D(touse[currentEar_m1], touse[currentEar], touse[currentEar_1]));
        touse.erase(currentEar);

        if (touse.size() == 3)
        {
            triangles.push_back(Triangle2D(touse[0], touse[1], touse[2]));
            break;
        }

        forrange(i, 0, ears.size())
        {
            if (ears[i] > currentEar)
                ears[i] = PreviousIndex(ears[i], (u32)touse.size());
        }

        forrange(i, 0, reflex.size())
        {
            if (reflex[i] > currentEar)
                reflex[i] = PreviousIndex(reflex[i], (u32)touse.size());
        }

        auto itConvex = std::find(convex.begin(), convex.end(), currentEar);
        AlwaysCheckedAssert(itConvex != convex.end());
        convex.erase(itConvex);
        forrange(i, 0, convex.size())
        {
            if (convex[i] > currentEar)
                convex[i] = PreviousIndex(convex[i], (u32)touse.size());
        }

        if (currentEar_m1 > currentEar)
            currentEar_m1 = PreviousIndex(currentEar_m1, (u32)touse.size());

        if (currentEar_1 > currentEar)
            currentEar_1 = PreviousIndex(currentEar_1, (u32)touse.size());

        UpdateAdjacentVertex(currentEar_m1, convex, reflex, ears, touse);
        UpdateAdjacentVertex(currentEar_1, convex, reflex, ears, touse);

        std::sort(convex.begin(), convex.end());
        std::sort(ears.begin(), ears.end());
    }

    return triangles;
}

struct PolygonSorter
{
    bool operator()(const Polygon2D& parA, const Polygon2D& parB) const
    {
        float maxXA = std::numeric_limits<float>::min();
        forrange(i, 0, parA.size())
        {
            const glm::vec2& point = parA[i];
            if (point.x > maxXA)
                maxXA = point.x;
        }

        float maxXB = std::numeric_limits<float>::min();
        forrange(i, 0, parB.size())
        {
            const glm::vec2& point = parB[i];
            if (point.x > maxXB)
                maxXB = point.x;
        }

        return maxXA > maxXB;
    }
};

std::vector<Triangle2D> PolygonTriangulator::Triangulate(const Polygon2D& parPolygon, const std::vector<Polygon2D> parPolygonHoles, Polygon2D& outExtentedPolygon)
{
    if (parPolygonHoles.empty())
    {
        outExtentedPolygon = parPolygon;
        return Triangulate(parPolygon);
    }

    Polygon2D polygonCopy = parPolygon;
    const bool isClockwise = polygonCopy.IsClockWise();
    if (isClockwise)
    {
        polygonCopy = polygonCopy.Revert();
    }

    std::vector<Polygon2D> polygonHolesCopy;
    polygonHolesCopy.insert(polygonHolesCopy.begin(), parPolygonHoles.begin(), parPolygonHoles.end());
    std::sort(polygonHolesCopy.begin(), polygonHolesCopy.end(), PolygonSorter{});

    foreachitemconst(hole, parPolygonHoles) { InsertHoleIntoPolygon(polygonCopy, hole); }

    outExtentedPolygon = polygonCopy;
    return Triangulate(polygonCopy);
}

std::vector<ECSEngine::Triangle2D> PolygonTriangulator::Triangulate(const Polygon2D& parPolygon, const std::vector<Polygon2D> parPolygonHoles)
{
    Polygon2D dummyOutput;
    return Triangulate(parPolygon, parPolygonHoles, dummyOutput);
}

} // namespace ECSEngine