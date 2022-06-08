#include "stdafx.h"

#include "CommonLoaders.h"

#include "Common/CameraManager.h"
#include "Common/Logger.h"
#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceFileDirectoryView.h"
#include "Common/ResourceHandle.h"
#include "Common/TimeManager.h"
#include "SceneManager.h"

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

    reverseforeachitem(subLoader, FSubLoaders) { subLoader->Shutdown(); }
    FSubLoaders.clear();
}

bool LoaderInitialiseCommonResources::VirtualInitialise()
{
    ILoader::VirtualInitialise();

    // Logger
    Logger::InitLogger();

    // Time
    TimeManager::Create();

    // CameraManager
    CameraManager::CreateIFP();

    // Scene manager
    SceneManager::CreateIFP();

    {
        AssertRelease(SceneManager::HasInstance());
        Resource r(FSceneManagerConfigFile);
        if (GlobalResourceCache::Instance().FCache->FileExists(&r))
        {
            auto handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&r);
            AssertRelease(handle != nullptr);
            ResourceBuffer buff = handle->GetResourceBuffer();
            std::istream istr(&buff, std::istream::in);
            cereal::JSONInputArchive archive(istr);
            archive(NAMEDPROPERTY("SceneManager", SceneManager::Instance()));
        }
    }

    return true;
}

void LoaderInitialiseCommonResources::VirtualShutdown()
{
    ILoader::VirtualShutdown();
    SceneManager::Destroy();
    CameraManager::Destroy();
    TimeManager::End();
    ECSEngine::Logger::ShutdownLogger();
}

} // namespace ECSEngine

CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ILoader, ECSEngine::LoaderInitialiseCommonResources);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ILoader, ECSEngine::LoadInitialiseSubLoaders);
