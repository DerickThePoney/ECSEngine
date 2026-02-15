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
        PROPERTYFIELD(Filename, "INVALID");
        PROPERTYFIELD(SoundGroup, ESoundGroup::MUSICS);
        PROPERTYFIELD(Stream, false);
        PROPERTYFIELD(Spatialized, false);
        PROPERTYFIELD(AllowPitchChanges, false);
        PROPERTYFIELD(bLoop, false);
    }

    std::string FFilename = "INVALID";
    ESoundGroup FSoundGroup = ESoundGroup::MUSICS;

    bool FStream = false;
    bool FSpatialized = false;
    bool FAllowPitchChanges = false;
    bool FbLoop = false;
};
} // namespace ECSEngine