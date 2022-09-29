#include "stdafx.h"

#include "PolygonPartitionner.h"

#include "Polygon.h"

#include <polypartition.cpp>

namespace ECSEngine
{
namespace
{
void ConvertECSToTPPL(const Polygon2D& parPolygon, TPPLPoly& parTPPL)
{
    parTPPL.Init((long)parPolygon.size());
    forrange(i, 0, parPolygon.size())
    {
        const vec2& point = parPolygon[i];
        parTPPL[(long)i].x = point.x;
        parTPPL[(long)i].y = point.y;
    }
}
void ConvertTPPLToECS(const TPPLPoly& parTPPL, Polygon2D& parPolygon)
{
    const u32 numPoints = (u32)parTPPL.GetNumPoints();
    parPolygon.reserve(numPoints);

    forrange(i, 0, numPoints)
    {
        const TPPLPoint& point = parTPPL[(long)i];
        parPolygon.push_back(vec2(point.x, point.y));
    }
}
} // namespace

std::vector<Polygon2D> PolygonPartionner::Partition(const Polygon2D& parPolygon, const std::vector<Polygon2D> parPolygonHoles)
{
    TPPLPolyList listA;
    TPPLPolyList listB;

    std::vector<Polygon2D> res;

    const Polygon2D mainPolygon = (parPolygon.IsClockWise()) ? parPolygon.Revert() : parPolygon;

    TPPLPoly mainPoly;
    ConvertECSToTPPL(mainPolygon, mainPoly);
    mainPoly.SetHole(false);
    listA.push_back(mainPoly);

    foreachitemconst(hole, parPolygonHoles)
    {
        const Polygon2D holePolygon = (hole.IsClockWise()) ? hole : hole.Revert();
        TPPLPoly holePoly;
        ConvertECSToTPPL(holePolygon, holePoly);
        holePoly.SetHole(true);
        listA.push_back(holePoly);
    }

    TPPLPartition partitionner;
    if (partitionner.RemoveHoles(&listA, &listB) != 1)
    {
        AssertNotReached();
        return res;
    }

    listA.clear();

    if (partitionner.ConvexPartition_HM(&listB, &listA) != 1)
    {
        AssertNotReached();
        return res;
    }

    // convert back
    res.reserve(listA.size());
    foreachitemconst(partition, listA)
    {
        Polygon2D poly;
        ConvertTPPLToECS(partition, poly);
        res.push_back(poly);
    }

    return res;
}

} // namespace ECSEngine
