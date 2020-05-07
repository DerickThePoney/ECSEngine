#pragma once
#include "Application/IGameplayUpdater.h"

namespace ECSEngine
{
class EntityTemplate;
class Scene;
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
    Scene* scene;
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