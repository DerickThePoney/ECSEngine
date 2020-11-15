#include "stdafx.h"

#include "HarvesterState.h"

namespace ECSEngine
{

const char* HarvesterState::GetName(const Type parState)
{
    switch (parState)
    {
    case IDLE:
        return "IDLE";
        break;
    case GOING_TO_PRODUCER:
        return "GOING_TO_PRODUCER";
        break;
    case HARVESTING:
        return "HARVESTING";
        break;
    case GOING_BACK_TO_COLONY:
        return "GOING_BACK_TO_COLONY";
        break;
    default:
        AssertNotReached();
        return "UNKNOWN_STATE";
        break;
    }
}

} // namespace ECSEngine