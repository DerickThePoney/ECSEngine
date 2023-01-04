#pragma once
#include "Common/Singleton.h"

namespace ECSEngine
{
namespace SoundResources
{
bool InitializeCache();
void DestroyCache();
} // namespace SoundResources

class ResourceCache;

class SoundResourceCache final : public Singleton<SoundResourceCache>
{
public:
    SoundResourceCache()
        : Singleton<SoundResourceCache>()
        , FCache(nullptr)
    {
    }

    ~SoundResourceCache()
    {
        if (FCache != nullptr)
        {
            delete FCache;
            FCache = nullptr;
        }
    }

public:
    ResourceCache* FCache;
};

} // namespace ECSEngine