#pragma once
#include "CameraMoverSystem.h"
#include "ECSGameplay_Specific/ColonyPeonsTaskAsignmentSystem.h"
#include "ECSGameplay_Specific/PeonLifeSpanSystem.h"
#include "ECSGameplay_Specific/PeonSpawnSystem.h"
#include "ECSGameplay_Specific/PeonsHarvestingSystem.h"
#include "ECSGameplay_Specific/ResourceProductionSystem.h"
#include "IScenarioUpdater.h"
#include "MovementSystem.h"
#include "OrientationSystem.h"
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

    CameraMoverSystem FCameraMoverSystem;
    MovementSystem FMovementSystem;
    RenderingSystem FRenderingSystem;

    ColonyPeonsTaskAssignmentSystem FColonyManagementSystem;
    PeonsHaverstingSystem FPeonHarvestingSytem;
    ResourceProductionSystem FProductionSystem;
    PeonSpawnSystem FPeonSpawnSystem;
    PeonLifeSpanSystem FPeonLifeSpanSystem;
};
} // namespace ECSEngine
