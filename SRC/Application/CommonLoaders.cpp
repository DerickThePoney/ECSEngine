#include "stdafx.h"

#include "CommonLoaders.h"

#include "Common/Logger.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceFileDirectoryView.h"
#include "Common/TimeManager.h"

namespace ECSEngine
{
bool LoadInitialiseSubLoaders::VirtualInitialise()
{
    ILoader::VirtualInitialise();

    bool res = true;

    for (size_t i = 0; i < FSubLoaders.size() && res; ++i)
    {
        res &= FSubLoaders[i]->Initialise();
    }

    return res;
}

void LoadInitialiseSubLoaders::VirtualShutdown()
{
    ILoader::VirtualShutdown();

    for (size_t i = FSubLoaders.size(); i > 0; i--)
    {
        FSubLoaders[i - 1]->Shutdown();
    }
}

bool LoaderInitialiseCommonResources::VirtualInitialise()
{
    ILoader::VirtualInitialise();

    // Logger
    Logger::InitLogger();

    // Time
    TimeManager::Start();

    // Global resources
    GlobalResourceCache::CreateIFP();
    GlobalResourceCache::Instance().FCache = new ResourceCache(10, new ResourceFileDirectoryView(FAssetDirectory));

    if (!GlobalResourceCache::Instance().FCache->Initialize())
    {
        AssertNotReachedMsg("Unable to init the resource cache!!");
        return false;
    }

    return true;
}

void LoaderInitialiseCommonResources::VirtualShutdown()
{
    ILoader::VirtualShutdown();
    GlobalResourceCache::Destroy();
    TimeManager::End();
    ECSEngine::Logger::ShutdownLogger();
}

} // namespace ECSEngine

CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ILoader, ECSEngine::LoaderInitialiseCommonResources);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ILoader, ECSEngine::LoadInitialiseSubLoaders);