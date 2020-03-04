#pragma once
#include "Application/IGameplayUpdater.h"
#include "Rendering/RenderingSystem.h"

namespace ECSEngine
{
class EntityTemplate;
class MeshMaterialApplicationUpdater
{
public:
    MeshMaterialApplicationUpdater();

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
};

class MeshMaterialApplicationUpdaterWrapper final : public IGameplayUpdater
{
public:
    MeshMaterialApplicationUpdaterWrapper();
    ~MeshMaterialApplicationUpdaterWrapper();

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
    MeshMaterialApplicationUpdater* FWrappedGameplayUpdater = nullptr;
};
} // namespace ECSEngine