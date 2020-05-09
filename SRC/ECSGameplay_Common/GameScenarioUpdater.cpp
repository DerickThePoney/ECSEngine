#include "stdafx.h"

#include "GameScenarioUpdater.h"

#include "Application/SceneScenario.h"

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
    FRenderingSystem.Init();
    FOrientationSystem.Init();

    FScenario->Initialise();
}

void GameScenarioUpdater::Destroy()
{
    FScenario->Destroy();
    delete FScenario;
    FScenario = nullptr;

    FOrientationSystem.Destroy();
    FRenderingSystem.Destroy();
}

void GameScenarioUpdater::Update()
{
    AssertRelease(FScenario != nullptr);
    FScenario->Update();

    FOrientationSystem.Update();
}

void GameScenarioUpdater::Render()
{
    AssertRelease(FScenario != nullptr);
    FScenario->Render();

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