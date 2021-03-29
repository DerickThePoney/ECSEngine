#pragma once
#include "CameraMoverSystem.h"
#include "ECSGameplay_Specific/ColonyBuildingSystem.h"
#include "ECSGameplay_Specific/ColonyFeedbackSystem.h"
#include "ECSGameplay_Specific/ColonyPeonsTaskAsignmentSystem.h"
#include "ECSGameplay_Specific/HousingSystem.h"
#include "ECSGameplay_Specific/PeonFeedingTimeSystem.h"
#include "ECSGameplay_Specific/PeonSpawnSystem.h"
#include "ECSGameplay_Specific/PeonsHarvestingSystem.h"
#include "ECSGameplay_Specific/ResourceProductionSystem.h"
#include "ECSGameplay_Specific/ResourceStatisticsUpdateSystem.h"
#include "ECSGameplay_Specific/UserInterfaceSystem.h"
#include "ECSGameplay_Specific/WorkSystem.h"
#include "IScenarioUpdater.h"
#include "MovementSystem.h"
#include "OrientationSystem.h"
#include "Rendering/GameRenderer.h"
#include "SynchroWithRenderSystem.h"

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

    CameraMoverSystem FCameraMoverSystem;
    MovementSystem FMovementSystem;
    SynchroWithRenderSystem FRenderingSystem;

    ColonyBuildingSystem FColonyBuildingSystem;
    ColonyPeonsTaskAssignmentSystem FColonyManagementSystem;
    PeonsHaverstingSystem FPeonHarvestingSytem;
    ResourceProductionSystem FProductionSystem;
    PeonSpawnSystem FPeonSpawnSystem;
    PeonFeedingTimeSystem FPeonLifeSpanSystem;
    ResourceStatisticsUpdateSystem FResourceStatsUpdateSystem;
    HousingSystem FHousingSystem;
    WorkSystem FWorkSystem;

    ColonyFeedbackSystem FColonyFeedbackSystem;

    UserInterfaceSystem FUserInterfaceSystem;

    float FTimeBeforeNextUpdate = 0.f;
};
} // namespace ECSEngine
