#include "stdafx.h"

#include "WorldIds.h"

namespace ECSEngine
{

namespace EEntityWorldsHelpers
{
const char* GetName(const EEntityWorlds parWorld)
{
    switch (parWorld)
    {
    case EEntityWorlds::STANDARD:
        return "STANDARD";
    case EEntityWorlds::COLONY:
        return "COLONY";
    case EEntityWorlds::CAMERA:
        return "CAMERA";
    case EEntityWorlds::RESOURCE_PROD:
        return "RESOURCE_PROD";
    case EEntityWorlds::PEONS:
        return "PEONS";
    case EEntityWorlds::BUILDINGS:
        return "BUILDINGS";
    default:
        AssertNotReached();
        return "UNKNOW";
        break;
    }
}
} // namespace EEntityWorldsHelpers
} // namespace ECSEngine
