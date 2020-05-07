#pragma once
#include "ECSGameplay_Common/EditorScene.h"
#include "Application/IGameplayUpdater.h"
#include "Application/Scene.h"
#include "Common/RenderingHandles.h"
#include "Common/RingBuffer.h"
#include "ECSCore/EntityId.h"
#include "ECSGameplay_Common/OrientationSystem.h"
#include "Rendering/EditorSceneRenderer.h"
#include "Rendering/RenderingSystem.h"
#include "Rendering/SceneObjectsPickingRenderer.h"

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
    EditorSceneRenderer editorSceneRenderer;
    SceneObjectsPickingRenderer editorSceneObjectPickingRenderer;

    // temp data
    const EntityTemplate* FTemplate;
    std::vector<ECSEngine::EntityId> FEntities;
    RingBuffer<float, 100> FFrameTimeBuffer;

    EditorScene scene;
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