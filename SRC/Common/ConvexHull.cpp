#include "stdafx.h"

#include "ConvexHull.h"

#include "Polygon.h"
#include "Sorting.h"

namespace ECSEngine
{

void ConvexHullKeepOriginalArray(std::vector<vec2>& parPoints, Polygon2D& outPolygon)
{
    std::vector<vec2> copy = parPoints;
    return ConvexHull(copy, outPolygon);
}

struct LexSort
{
    constexpr auto operator()(const vec2& a, const vec2& b) const { return a.x < b.x || ((a.x == b.x) && (a.y < b.y)); }
};

void ConvexHull(std::vector<vec2>& parPoints, Polygon2D& outPolygon)
{
    // 1- SortPoints lexicographically
    InPlaceSorting<std::vector<vec2>, vec2>(parPoints, 0, parPoints.size() - 1, LexSort{});

    // 2- Lupper
    // 2.a- Push the first two points
    outPolygon.reserve(parPoints.size());
    outPolygon.push_back(parPoints[0]);
    outPolygon.push_back(parPoints[1]);

    // 2.b loop over the vertices
    forrange(i, 2, parPoints.size())
    {
        outPolygon.push_back(parPoints[i]);
        while (outPolygon.size() > 2)
        {
            size_t idx0 = outPolygon.size() - 3;
            size_t idx1 = outPolygon.size() - 2;
            size_t idx2 = outPolygon.size() - 1;

            vec2 vec1 = outPolygon[idx1] - outPolygon[idx0];
            vec2 vec2 = outPolygon[idx2] - outPolygon[idx1];

            // if right turn terminate
            if ((vec1.x * vec2.y - vec1.y * vec2.x) > 0.f)
                break;

            // if left turn or colinear remove Point idx2
            outPolygon.erase(idx1);
        }
    }

    // 2- Llower
    // 2.a- Push the first two points
    outPolygon.push_back(parPoints[parPoints.size() - 2]);

    // 2.b loop over the vertices
    reverseforrange(i, 0, parPoints.size() - 3)
    {
        outPolygon.push_back(parPoints[i]);
        while (outPolygon.size() > 2)
        {
            size_t idx0 = outPolygon.size() - 3;
            size_t idx1 = outPolygon.size() - 2;
            size_t idx2 = outPolygon.size() - 1;

            vec2 vec1 = outPolygon[idx1] - outPolygon[idx0];
            vec2 vec2 = outPolygon[idx2] - outPolygon[idx1];

            // if right turn terminate
            if ((vec1.x * vec2.y - vec1.y * vec2.x) > 0.f)
                break;

            // if left turn or colinear remove Point idx2
            outPolygon.erase(idx1);
        }
    }

    // concatenate both lists
    outPolygon.resize(outPolygon.size());
}

} // namespace ECSEngine
