#pragma once
#include "IScenarioUpdater.h"

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
};
} // namespace ECSEngine
