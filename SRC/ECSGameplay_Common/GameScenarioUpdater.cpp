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
}

void GameScenarioUpdater::Destroy()
{
    delete FScenario;
    FScenario = nullptr;
}

void GameScenarioUpdater::Update()
{
    AssertRelease(FScenario != nullptr);
}

void GameScenarioUpdater::Render()
{
    AssertRelease(FScenario != nullptr);
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