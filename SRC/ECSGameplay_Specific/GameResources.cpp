#include "stdafx.h"

#include "GameResources.h"

#include "Common/SavingSystemImplementation.h"

namespace ECSEngine
{
namespace GameResource
{
const char* GetName(const Type parResource)
{
    switch (parResource)
    {
#define RESOURCE(NAME)                                                                                                                                                             \
    case NAME:                                                                                                                                                                     \
        return #NAME;
#include "GameResources.inl"
#undef RESOURCE
    case LENGTH:
        return "UNKNOWN RESOURCE";
    default:
        AssertNotReached();
        return "UNKNOWN RESOURCE";
        break;
    }
}
} // namespace GameResource

IMPLEMENT_SAVELOAD_ABILITIES_FREEFUNC(GameResource::Type);
} // namespace ECSEngine
