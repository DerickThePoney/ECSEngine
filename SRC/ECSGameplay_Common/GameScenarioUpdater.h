#pragma once
#include "CameraMoverSystem.h"
#include "ECSGameplay_Specific/ColonyBuildingSystem.h"
#include "ECSGameplay_Specific/ColonyFeedbackSystem.h"
#include "ECSGameplay_Specific/ResourceProductionSystem.h"
#include "ECSGameplay_Specific/ResourceStatisticsUpdateSystem.h"
#include "ECSGameplay_Specific/StorageFeedbackDrawer.h"
#include "ECSGameplay_Specific/StorageSlotSystem.h"
#include "ECSGameplay_Specific/UserInterfaceSystem.h"
#include "IScenarioUpdater.h"
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
    void RealtimeUpdate() override;
    void GameplayUpdate() override;
    void UIUpdate() override;
    void DebugRender() override;
    void Render() override;

    void SetScenario(const std::string& parScenarioFile);

private:
    SceneScenario* FScenario;

    CameraMoverSystem FCameraMoverSystem;
    SynchroWithRenderSystem FRenderingSystem;

    ColonyBuildingSystem FColonyBuildingSystem;
    ResourceProductionSystem FRawResourceProductionSystem;
    ResourceStatisticsUpdateSystem FResourceStatsUpdateSystem;
    StorageSlotSystem FStorageSlotSystem;

    ColonyFeedbackSystem FColonyFeedbackSystem;
    StorageFeedbackDrawer FStorageFeedback;

    UserInterfaceSystem FUserInterfaceSystem;

    float FTimeBeforeNextUpdate = 0.f;
};
} // namespace ECSEngine
