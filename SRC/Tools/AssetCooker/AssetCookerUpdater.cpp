#include "stdafx.h"

#include "AssetCookerUpdater.h"

#include "Common/Logger.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceFile.h"
#include "Common/Timer.h"
#include "ResourcesCooking.h"
#include "Tools/AssimpWrapper/AssimpMeshDataLoading.h"
#include "assimp/scene.h"

namespace ECSEngine
{

AssetCookerUpdater::AssetCookerUpdater()
    : FShouldClose(false)
{
}

void AssetCookerUpdater::Initialise()
{
    LOG_COOKING("Retrieve mesh files");
    GlobalResourceCache::Instance().FCache->GetFileSystem()->ListResourceFiles("*.fbx", FMeshFiles);

    LOG_COOKING("Retrieve texture files");
    GlobalResourceCache::Instance().FCache->GetFileSystem()->ListResourceFiles("*.texturebank", FTextureBanksFiles);
}

void AssetCookerUpdater::Shutdown()
{
}

bool AssetCookerUpdater::CheckShouldFinish()
{
    return FShouldClose;
}

void AssetCookerUpdater::StartUpdate()
{
}

void AssetCookerUpdater::Update()
{
    std::vector<std::pair<std::string, float>> timings;
    {
        Timer t(true);
        CookMeshes(FMeshFiles);
        const float meshCookingTimings = t.Stop();
        timings.push_back({ "Mesh cooking", meshCookingTimings });
    }

    {
        Timer t(true);
        CookTextures(FTextureBanksFiles);
        const float textureCookingTimings = t.Stop();
        timings.push_back({ "Texture cooking", textureCookingTimings });
    }

    FShouldClose = true;
}

void AssetCookerUpdater::Render()
{
}

void AssetCookerUpdater::EndUpdate()
{
}

AssetCookerUpdaterWrapper::AssetCookerUpdaterWrapper()
    : FWrappedGameplayUpdater(nullptr)
{
}

AssetCookerUpdaterWrapper::~AssetCookerUpdaterWrapper()
{
    AssertRelease(FWrappedGameplayUpdater == nullptr);
}

void AssetCookerUpdaterWrapper::Initialise()
{
    AssertRelease(FWrappedGameplayUpdater == nullptr);
    FWrappedGameplayUpdater = new AssetCookerUpdater();

    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->Initialise();
}

void AssetCookerUpdaterWrapper::Shutdown()
{
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->Shutdown();

    delete FWrappedGameplayUpdater;
    FWrappedGameplayUpdater = nullptr;
}

bool AssetCookerUpdaterWrapper::CheckShouldFinish()
{
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    return FWrappedGameplayUpdater->CheckShouldFinish();
}

void AssetCookerUpdaterWrapper::StartUpdate()
{
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->StartUpdate();
}

void AssetCookerUpdaterWrapper::Update()
{
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->Update();
}

void AssetCookerUpdaterWrapper::Render()
{
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->Render();
}

void AssetCookerUpdaterWrapper::EndUpdate()
{
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->EndUpdate();
}

} // namespace ECSEngine

CEREAL_REGISTER_TYPE(ECSEngine::AssetCookerUpdaterWrapper);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::IGameplayUpdater, ECSEngine::AssetCookerUpdaterWrapper);