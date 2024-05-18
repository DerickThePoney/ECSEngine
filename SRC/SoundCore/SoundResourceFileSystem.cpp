#include "stdafx.h"

#include "SoundResourceFileSystem.h"

#include "Common/DataPackDirectoryView.h"
#include "Common/MainOptions.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceFileDirectoryView.h"

namespace ECSEngine
{
namespace SoundResources
{

bool InitializeCache()
{
    SoundResourceCache::CreateIFP();

#ifdef COMPILE_FINAL
    SoundResourceCache::Instance().FCache = new ResourceCache(100, new DataPackDirectoryView(std::string(Configuration::SoundDatapackDirectory) + ".datapack"));
#else
    if (Options.NoDatapack)
        SoundResourceCache::Instance().FCache = new ResourceCache(100, new ResourceFileDirectoryView(Configuration::SoundsDirectory));
    else
        SoundResourceCache::Instance().FCache = new ResourceCache(100, new DataPackDirectoryView(std::string(Configuration::SoundDatapackDirectory) + ".datapack"));
#endif

    if (!SoundResourceCache::Instance().FCache->Initialize())
    {
        AssertNotReachedMsg("Unable to init the resource cache!!");
        return false;
    }
    return true;
}

void DestroyCache()
{
    GlobalResourceCache::Destroy();
}

} // namespace SoundResources
} // namespace ECSEngine