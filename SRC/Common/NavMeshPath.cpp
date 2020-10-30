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

void NavMeshPath::push_back(NavMeshVertex* parVertex)
{
#ifdef PERFORM_SECURITY_CHECKS
    AssertExistsAndNoDoublon(parVertex);
#endif // PERFORM_SECURITY_CHECKS
    FWaypoints.push_back(parVertex);
}

#ifdef PERFORM_SECURITY_CHECKS
void NavMeshPath::AssertExistsAndNoDoublon(NavMeshVertex* parVertex) const
{
    AssertRelease(parVertex != nullptr);
    foreachitemconst(vertex, FWaypoints) { AssertRelease(parVertex != vertex); }
}
#endif
} // namespace Navigation
} // namespace ECSEngine
