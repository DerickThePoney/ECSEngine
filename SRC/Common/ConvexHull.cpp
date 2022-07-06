#include "stdafx.h"

#include "ConvexHull.h"

#include "Polygon.h"
#include "Sorting.h"

namespace ECSEngine
{

void ConvexHullKeepOriginalArray(std::vector<glm::vec2>& parPoints, Polygon2D& outPolygon)
{
    std::vector<glm::vec2> copy = parPoints;
    return ConvexHull(copy, outPolygon);
}

struct LexSort
{
    constexpr auto operator()(const glm::vec2& a, const glm::vec2& b) const { return a.x < b.x || ((a.x == b.x) && (a.y < b.y)); }
};

void ConvexHull(std::vector<glm::vec2>& parPoints, Polygon2D& outPolygon)
{
    // 1- SortPoints lexicographically
    InPlaceSorting<std::vector<glm::vec2>, glm::vec2>(parPoints, 0, parPoints.size() - 1, LexSort{});

    // 2- Lupper
    // 2.a- Push the first two points
    std::vector<glm::vec2> lupper;
    lupper.reserve(parPoints.size());
    lupper.push_back(parPoints[0]);
    lupper.push_back(parPoints[1]);

    // 2.b loop over the vertices
    forrange(i, 2, parPoints.size())
    {
        lupper.push_back(parPoints[i]);
        while (lupper.size() > 2)
        {
            size_t idx0 = lupper.size() - 3;
            size_t idx1 = lupper.size() - 2;
            size_t idx2 = lupper.size() - 1;

            glm::vec2 vec1 = lupper[idx1] - lupper[idx0];
            glm::vec2 vec2 = lupper[idx2] - lupper[idx1];

            // if right turn terminate
            if ((vec1.x * vec2.y - vec1.y * vec2.x) > 0.f)
                break;

            // if left turn or colinear remove Point idx2
            lupper.erase(lupper.begin() + idx1);
        }
    }

    // 2- Llower
    // 2.a- Push the first two points
    std::vector<glm::vec2> llower;
    llower.reserve(parPoints.size());
    llower.push_back(parPoints[parPoints.size() - 1]);
    llower.push_back(parPoints[parPoints.size() - 2]);

    // 2.b loop over the vertices
    reverseforrange(i, 0, parPoints.size() - 3)
    {
        llower.push_back(parPoints[i]);
        while (llower.size() > 2)
        {
            size_t idx0 = llower.size() - 3;
            size_t idx1 = llower.size() - 2;
            size_t idx2 = llower.size() - 1;

            glm::vec2 vec1 = llower[idx1] - llower[idx0];
            glm::vec2 vec2 = llower[idx2] - llower[idx1];

            // if right turn terminate
            if ((vec1.x * vec2.y - vec1.y * vec2.x) > 0.f)
                break;

            // if left turn or colinear remove Point idx2
            llower.erase(llower.begin() + idx1);
        }
    }

    // concatenate both lists
    outPolygon.reserve(lupper.size() + llower.size() - 2);
    outPolygon.append(lupper);
    forrange(i, 1, llower.size() - 1) { outPolygon.push_back(llower[i]); }
}

} // namespace ECSEngine
