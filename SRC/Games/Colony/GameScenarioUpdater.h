#pragma once
#include "ECSGameplay_Common/CameraMoverSystem.h"
#include "Common/SavingSystemDeclaration.h"
#include "ColonyBuildingSystem.h"
#include "ColonyFeedbackSystem.h"
#include "ResourceProductionSystem.h"
#include "ResourceStatisticsUpdateSystem.h"
#include "StorageFeedbackDrawer.h"
#include "StorageSlotSystem.h"
#include "UserInterfaceSystem.h"
#include "ECSGameplay_Common/IScenarioUpdater.h"
#include "ECSGameplay_Common/PhysicsUpdateSystem.h"
#include "ECSGameplay_Common/SynchroWithRenderSystem.h"

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

    void SetScenario(const std::string& parScenarioFile, bool bForceReload = false);

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
