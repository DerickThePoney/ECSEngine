#include "stdafx.h"

#include "NavMeshUtilities.h"

#include "IntersectionRoutines.h"
#include "NavMesh.h"

namespace ECSEngine
{
namespace Navigation
{
namespace NavMeshHelpers
{

bool NavMeshSegmentIntersection2D(const NavMesh& parNavMesh, const Segment2D& parSegment)
{
    if (Intersection::SegmentPolygonIntersections2D_StopAtFirstIntersection(parSegment, parNavMesh.MainPolygon(), true, true))
        return true;

    MemoryView<const Polygon2D> holes = parNavMesh.Holes();
    foreachitemconst(hole, holes)
    {
        if (Intersection::SegmentPolygonIntersections2D_StopAtFirstIntersection(parSegment, hole, true, false))
            return true;
    }
    return false;
}

} // namespace NavMeshHelpers
} // namespace Navigation
} // namespace ECSEngine
