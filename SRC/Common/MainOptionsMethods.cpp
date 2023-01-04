#include "stdafx.h"

#include "DataPackDirectoryView.h"
#include "MainOptions.h"
#include "ResourceCache.h"
#include "ResourceFileDirectoryView.h"

namespace ECSEngine
{
bool InitialiseGlobalCache()
{
    // Global resources
    GlobalResourceCache::CreateIFP();

#ifdef COMPILE_FINAL
    GlobalResourceCache::Instance().FCache = new ResourceCache(10, new DataPackDirectoryView(std::string(Configuration::AssetsDatapackDirectory) + ".datapack"));
#else
    if (Options.NoDatapack)
        GlobalResourceCache::Instance().FCache = new ResourceCache(10, new ResourceFileDirectoryView(Configuration::AssetsDirectory));
    else
        GlobalResourceCache::Instance().FCache = new ResourceCache(10, new DataPackDirectoryView(std::string(Configuration::AssetsDatapackDirectory) + ".datapack"));
#endif

    if (!GlobalResourceCache::Instance().FCache->Initialize())
    {
        AssertNotReachedMsg("Unable to init the resource cache!!");
        return false;
    }
    return true;
}

void DestroyGlobalCache()
{
    GlobalResourceCache::Destroy();
}

} // namespace ECSEngine