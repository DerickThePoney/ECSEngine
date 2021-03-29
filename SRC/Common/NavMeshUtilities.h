#pragma once
#include "Segment.h"

namespace ECSEngine
{
namespace Navigation
{
class NavMesh;
namespace NavMeshHelpers
{
bool NavMeshSegmentIntersection2D(const NavMesh& parNavMesh, const Segment2D& parSegment);
}
} // namespace Navigation
} // namespace ECSEngine
