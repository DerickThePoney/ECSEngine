#include "stdafx.h"

#include "NavMeshPathSmoother.h"

#include "NavMesh.h"
#include "NavMeshPath.h"
#include "NavMeshUtilities.h"
#include "Segment.h"

namespace ECSEngine
{
namespace Navigation
{

void NavMeshPathSmoother::SmoothPath(const NavMesh& parNavMesh, NavMeshPath& outPath)
{
    AlwaysCheckedAssert(outPath.isValid());
    if (outPath.waypoints_size() < 2)
    {
        // if only 1 waypoint, we know there is no visibility, because we had to go through the process of finding waypoints
        return;
    }

    NavMeshPath newNavMeshPath(outPath.Start(), outPath.End());
    newNavMeshPath.SetValid(true); // We know we are :)

    glm::vec2 curentSegmentStart = outPath.Start();

    // start at the second waypoint. LoS between start and first waypoint is assumed :)
    u32 currentIndex = 1;
    bool success = false;
    while (!success)
    {
        bool intersected = false;
        forrange(i, currentIndex, outPath.waypoints_size())
        {
            Segment2D s(curentSegmentStart, outPath[i]);
            if (NavMeshHelpers::NavMeshSegmentIntersection2D(parNavMesh, s))
            {
                // the segment intersects in the navMesh, there is no direct point... push the last point in the new path
                intersected = true;
                newNavMeshPath.push_back(outPath[i - 1]);
                curentSegmentStart = outPath[i - 1];
                currentIndex = (u32)i + 1;
                break;
            }
        }

        // if intersected, just go on with the process
        if (intersected)
            continue;

        // otherwise we reached the end of the path without no intersection. Check intersection with the end point. If yes, add the last path waypoint
        if (NavMeshHelpers::NavMeshSegmentIntersection2D(parNavMesh, Segment2D(curentSegmentStart, outPath.End())))
        {
            newNavMeshPath.push_back(outPath[outPath.waypoints_size() - 1]);
        }
        success = true;
    }

    outPath = newNavMeshPath;
}

} // namespace Navigation
} // namespace ECSEngine
