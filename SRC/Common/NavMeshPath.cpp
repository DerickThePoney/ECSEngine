#include "stdafx.h"

#include "NavMeshPath.h"

namespace ECSEngine
{
namespace Navigation
{

bool NavMeshPath::isValid() const
{
    return FValid && FStart != FEnd;
}

void NavMeshPath::push_back(glm::vec2 parWaypoint)
{
#ifdef PERFORM_SECURITY_CHECKS
    AssertExistsAndNoDoublon(parWaypoint);
#endif // PERFORM_SECURITY_CHECKS
    FWaypoints.push_back(parWaypoint);
}

#ifdef PERFORM_SECURITY_CHECKS
void NavMeshPath::AssertExistsAndNoDoublon(glm::vec2 parWaypoint) const
{
    foreachitemconst(vertex, FWaypoints) { AssertRelease(parWaypoint != vertex); }
}
#endif
} // namespace Navigation
} // namespace ECSEngine
