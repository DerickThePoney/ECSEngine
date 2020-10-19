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
    Triangle2D currentTri(parPolygon[i], parPolygon[i_m1], parPolygon[i_1]);
    while (idx != i_m1)
    {
        bool thisRes = Intersect::PointTriangle2D(currentTri, parPolygon[idx]);
        if (thisRes)
            return false;

        idx = NextIndex(idx, (u32)parPolygon.size());
    }

    return true;
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

        const glm::vec3 pi_m1 = glm::vec3(parPolygon[i_m1], 0.f);
        const glm::vec3 pi = glm::vec3(parPolygon[idx], 0.f);
        const glm::vec3 pi_1 = glm::vec3(parPolygon[i_p1], 0.f);

        float s = glm::sign(glm::cross(pi_m1 - pi, pi_1 - pi).z);

        if (s < 0.f)
        {
            // on devient convex, ear test + lists update
            reflex.erase(itReflex);
            convex.push_back(idx);

            if (EarTest(i_m1, idx, i_p1, parPolygon))
                ears.push_back(idx);
        }
    }
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
        const glm::vec3 pi_m1 = glm::vec3(touse[i_m1], 0.f);
        const glm::vec3 pi = glm::vec3(touse[i], 0.f);
        const glm::vec3 pi_1 = glm::vec3(touse[i_p1], 0.f);

        float s = glm::sign(glm::cross(pi_m1 - pi, pi_1 - pi).z);
        if (s >= 0.f)
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

        triangles.push_back(Triangle2D(touse[currentEar], touse[currentEar_m1], touse[currentEar_1]));
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
        std::sort(ears.end(), ears.end());
    }

    return triangles;
}

} // namespace ECSEngine