#include "stdafx.h"

#include "GameScenarioUpdater.h"

#include "Application/SceneScenario.h"
#include "ECSCore/WorldManager.h"

namespace ECSEngine
{

GameScenarioUpdater::GameScenarioUpdater()
    : FScenario(nullptr)
{
}

GameScenarioUpdater::~GameScenarioUpdater()
{
    delete FScenario;
}

void GameScenarioUpdater::Initialise()
{
    AssertRelease(FScenario != nullptr);
    FCameraMoverSystem.Init();
    FRenderingSystem.Init();

    FScenario->Initialise();
}

void GameScenarioUpdater::Destroy()
{
    WorldManager::Instance().DestroyAllRemainingEntities();

    FScenario->Destroy();
    delete FScenario;
    FScenario = nullptr;

    FCameraMoverSystem.Destroy();
    FRenderingSystem.Destroy();
}

void GameScenarioUpdater::Update()
{
    AssertRelease(FScenario != nullptr);
    FScenario->Update();
    FCameraMoverSystem.Update();
}

void GameScenarioUpdater::Render()
{
    AssertRelease(FScenario != nullptr);
    // FScenario->Render();

    FRenderingSystem.Update();
}

void GameScenarioUpdater::SetScenario(const std::string& parScenarioFile)
{
    AssertRelease(FScenario == nullptr);
    std::ifstream ofstr(parScenarioFile);
    cereal::JSONInputArchive ar(ofstr);

    FScenario = new SceneScenario();
    ar(*FScenario);
}

} // namespace ECSEngine