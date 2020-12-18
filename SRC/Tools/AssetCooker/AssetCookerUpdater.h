#pragma once
#include "Application/IGameplayUpdater.h"

namespace ECSEngine
{
class EntityTemplate;
class AssetCookerUpdater
{
public:
    AssetCookerUpdater();

    void Initialise();

    void Shutdown();

    bool CheckShouldFinish();

    void StartUpdate();

    void Update();

    void Render();

    void EndUpdate();

    template<typename Archive>
    void serialize(Archive& ar)
    {
    }

private:
    bool FShouldClose;
    std::vector<std::string> FMeshFiles;
    std::vector<std::string> FTextureBanksFiles;
    std::vector<std::string> FShaderFiles;
};

class AssetCookerUpdaterWrapper final : public IGameplayUpdater
{
public:
    AssetCookerUpdaterWrapper();
    ~AssetCookerUpdaterWrapper();

    void Initialise() override;

    void Shutdown() override;

    bool CheckShouldFinish() override;

    void StartUpdate() override;

    void Update() override;

    void Render() override;

    void EndUpdate() override;

    template<typename Archive>
    void serialize(Archive& ar)
    {
    }

private:
    AssetCookerUpdater* FWrappedGameplayUpdater = nullptr;
};
} // namespace ECSEngine