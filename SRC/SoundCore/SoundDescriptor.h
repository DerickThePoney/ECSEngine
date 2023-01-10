#pragma once
#include "Common/PoolAllocator.h"
#include "SoundGroups.h"

namespace ECSEngine
{
struct SoundDescriptor
{
    DECLARE_POOL_ALLOCATED(SoundDescriptor);

public:
    void DrawInEditor();

    SERIALIZE()
    {
        PROPERTYFIELD(FFilename, "INVALID");
        PROPERTYFIELD(FSoundGroup, ESoundGroup::MUSICS);
        PROPERTYFIELD(FStream, false);
        PROPERTYFIELD(FSpatialized, false);
        PROPERTYFIELD(FAllowPitchChanges, false);
        PROPERTYFIELD(FbLoop, false);
    }

    std::string FFilename = "INVALID";
    ESoundGroup FSoundGroup = ESoundGroup::MUSICS;

    bool FStream = false;
    bool FSpatialized = false;
    bool FAllowPitchChanges = false;
    bool FbLoop = false;
};
} // namespace ECSEngine