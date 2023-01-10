#include "stdafx.h"

#include "SoundGroups.h"

namespace ECSEngine
{
namespace SoundGroup
{
char* GetName(ESoundGroup parValue)
{
    switch (parValue)
    {
    case ECSEngine::ESoundGroup::MUSICS:
        return "MASTER";
        break;
    case ECSEngine::ESoundGroup::EFFECTS:
        return "EFFECTS";
        break;
    case ECSEngine::ESoundGroup::LENGTH:
        return "LENGTH";
        break;
    case ECSEngine::ESoundGroup::MASTER:
        return "MASTER";
        break;
    default:
        AssertNotReached();
        return "UNKNOWN";
        break;
    }
}
} // namespace SoundGroup
} // namespace ECSEngine
