#include "stdafx.h"

#include "MainOptions.h"
#include "ResourceCache.h"
#include "ResourceFileDirectoryView.h"

namespace ECSEngine
{
bool InitialiseGlobalCache()
{
    // Global resources
    GlobalResourceCache::CreateIFP();
    GlobalResourceCache::Instance().FCache = new ResourceCache(10, new ResourceFileDirectoryView(Configuration::AssetsDirectory));

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