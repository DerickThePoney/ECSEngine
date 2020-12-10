#pragma once
#include "CameraMoverSystem.h"
#include "ECSGameplay_Specific/ColonyPeonsTaskAsignmentSystem.h"
#include "ECSGameplay_Specific/PeonFeedingTimeSystem.h"
#include "ECSGameplay_Specific/PeonSpawnSystem.h"
#include "ECSGameplay_Specific/PeonsHarvestingSystem.h"
#include "ECSGameplay_Specific/ResourceProductionSystem.h"
#include "ECSGameplay_Specific/ResourceStatisticsUpdateSystem.h"
#include "ECSGameplay_Specific/UserInterfaceSystem.h"
#include "IScenarioUpdater.h"
#include "MovementSystem.h"
#include "OrientationSystem.h"
#include "Rendering/GameRenderer.h"
#include "Rendering/RenderingSystem.h"

namespace ECSEngine
{
class SceneScenario;
class GameScenarioUpdater : public IScenarioUpdater
{
public:
    GameScenarioUpdater();
    virtual ~GameScenarioUpdater();

    void Initialise() override;
    void Destroy() override;
    void Update() override;
    void Render() override;

    void SetScenario(const std::string& parScenarioFile);

private:
    SceneScenario* FScenario;

    Rendering::GameRenderer FGameRenderer;

    CameraMoverSystem FCameraMoverSystem;
    MovementSystem FMovementSystem;
    RenderingSystem FRenderingSystem;

    ColonyPeonsTaskAssignmentSystem FColonyManagementSystem;
    PeonsHaverstingSystem FPeonHarvestingSytem;
    ResourceProductionSystem FProductionSystem;
    PeonSpawnSystem FPeonSpawnSystem;
    PeonFeedingTimeSystem FPeonLifeSpanSystem;
    ResourceStatisticsUpdateSystem FResourceStatsUpdateSystem;
    UserInterfaceSystem FUserInterfaceSystem;
};
} // namespace ECSEngine
