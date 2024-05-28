#pragma once
#include "CameraMoverSystem.h"
#include "Common/SavingSystemDeclaration.h"
#include "ECSGameplay_Specific/ColonyBuildingSystem.h"
#include "ECSGameplay_Specific/ColonyFeedbackSystem.h"
#include "ECSGameplay_Specific/ResourceProductionSystem.h"
#include "ECSGameplay_Specific/ResourceStatisticsUpdateSystem.h"
#include "ECSGameplay_Specific/StorageFeedbackDrawer.h"
#include "ECSGameplay_Specific/StorageSlotSystem.h"
#include "ECSGameplay_Specific/UserInterfaceSystem.h"
#include "IScenarioUpdater.h"
#include "PhysicsUpdateSystem.h"
#include "SynchroWithRenderSystem.h"

namespace ECSEngine
{
class SceneScenario;
class GameScenarioUpdater : public IScenarioUpdater
{
    DECLARE_SAVELOAD_ABILITIES();

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
    void EndUpdate() override;

    void SetScenario(const std::string& parScenarioFile);

private:
    SceneScenario* FScenario;
    std::string FScenarioFileName;

    CameraMoverSystem FCameraMoverSystem;
    SynchroWithRenderSystem FRenderingSystem;

    PhysicsUpdateSystem FPhysicsSystem;

    ColonyBuildingSystem FColonyBuildingSystem;
    ResourceProductionSystem FRawResourceProductionSystem;
    ResourceStatisticsUpdateSystem FResourceStatsUpdateSystem;
    StorageSlotSystem FStorageSlotSystem;

    ColonyFeedbackSystem FColonyFeedbackSystem;
    StorageFeedbackDrawer FStorageFeedback;

    UserInterfaceSystem FUserInterfaceSystem;
};
} // namespace ECSEngine
