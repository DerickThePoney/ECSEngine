#pragma once
#include "ECSGameplay_Common/CameraMoverSystem.h"
#include "ECSGameplay_Common/IScenarioUpdater.h"
#include "ECSGameplay_Common/PhysicsUpdateSystem.h"
#include "ECSGameplay_Common/SynchroWithRenderSystem.h"
#include "RocketControllerSystem.h"

namespace ECSEngine
{
class SceneScenario;

class OrbitalScenarioUpdater : public IScenarioUpdater
{
public:
    OrbitalScenarioUpdater();
    ~OrbitalScenarioUpdater() override;

    void Initialise() override;
    void Destroy() override;
    void RealtimeUpdate() override;
    void GameplayUpdate() override;
    void UIUpdate() override;
    void DebugRender() override;
    void Render() override;
    void EndUpdate() override;

    void SetScenario(const std::string& parScenarioFile, bool bForceReload = false);

private:
    SceneScenario* FScenario = nullptr;
    std::string FScenarioFileName;

    CameraMoverSystem FCameraMoverSystem;
    SynchroWithRenderSystem FRenderingSystem;
    PhysicsUpdateSystem FPhysicsSystem;
    RocketControllerSystem FRocketSystem;
};
} // namespace ECSEngine
