#pragma once
#include "Application/IGameplayUpdater.h"
#include "Common/MeshHandle.h"
#include "Common/RingBuffer.h"
#include "ECSCore/EntityId.h"
#include "ECSGameplay_Common/OrientationSystem.h"
#include "Rendering/RenderingSystem.h"

namespace ECSEngine
{
class EntityTemplate;
class ApplicationUpdater
{
public:
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
    OrientationSystem orientationSystem;
    RenderingSystem renderSystem;

    // temp data
    const EntityTemplate* FTemplate;
    Rendering::MeshHandle FMeshHandle;
    std::vector<ECSEngine::EntityId> FEntities;
    RingBuffer<float, 100> FFrameTimeBuffer;
};

class ApplicationUpdaterWrapper final : public IGameplayUpdater
{
public:
    ApplicationUpdaterWrapper();
    ~ApplicationUpdaterWrapper();

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
    ApplicationUpdater* FWrappedGameplayUpdater = nullptr;
};
} // namespace ECSEngine