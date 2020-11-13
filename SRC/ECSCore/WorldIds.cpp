#include "stdafx.h"

#include "WorldIds.h"

namespace ECSEngine
{
namespace Worlds
{

const char* GetName(const Type parWorld)
{
    switch (parWorld)
    {
    case STANDARD:
        return "STANDARD";
    case COLONY:
        return "COLONY";
    case CAMERA:
        return "CAMERA";
    default:
        AssertNotReached();
        return "UNKNOW";
        break;
    }
}

} // namespace Worlds
} // namespace ECSEngine