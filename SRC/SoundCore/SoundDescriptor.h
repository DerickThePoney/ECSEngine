#pragma once
#include "SoundGroups.h"

#include <string>

namespace ECSEngine
{
struct SoundDescriptor
{
    std::string FFilename; // TODO - MAYBE SOMETHING ELSE
    ESoundGroup SoundGroup = ESoundGroup::MUSICS;

    bool bStream = false;
    bool bSpatialized = false;
    bool bAllowPitchChanges = false;
    bool bLoop = false;
};
} // namespace ECSEngine