#include "stdafx.h"

#include "OrbitalLoader.h"

#include "ECSCore/EntityWorld.h"
#include "ECSCore/WorldIds.h"
#include "ECSCore/WorldManager.h"
#include "ECSGameplay_Common/ScenarioFactory.h"
#include "FuelTankModule.h"
#include "NozzleEngineModule.h"
#include "OrbitalScenarioUpdater.h"
#include "PlanetGravityModule.h"
#include "RocketCameraMoverModule.h"
#include "RocketManagerModule.h"

CEREAL_REGISTER_TYPE(ECSEngine::OrbitalLoader);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ILoader, ECSEngine::OrbitalLoader);

namespace ECSEngine
{
namespace
{
IScenarioUpdater* CreateOrbitalScenario(const std::string& scenarioFile, bool forceReload)
{
    OrbitalScenarioUpdater* scene = new OrbitalScenarioUpdater();
    scene->SetScenario(scenarioFile, forceReload);
    return scene;
}
} // namespace

bool OrbitalLoader::VirtualInitialise()
{
    ILoader::VirtualInitialise();

    EntityWorld* world = WorldManager::Instance().GetWorldIFP(EEntityWorlds::STANDARD);
    AssertRelease(world != nullptr);
    world->AddController<RocketManagerModule>();
    world->AddController<PlanetGravityModule>();
    world->AddController<NozzleEngineModule>();
    world->AddController<FuelTankModule>();

    world = WorldManager::Instance().GetWorldIFP(EEntityWorlds::CAMERA);
    AssertRelease(world != nullptr);
    world->AddController<RocketCameraMoverModule>();

    SetScenarioFactory(&CreateOrbitalScenario);
    return true;
}

void OrbitalLoader::VirtualShutdown()
{
    ILoader::VirtualShutdown();
}
} // namespace ECSEngine
