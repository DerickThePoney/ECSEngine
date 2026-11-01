#pragma once

#include <string>

namespace ECSEngine
{
class IScenarioUpdater;

using ScenarioFactoryFn = IScenarioUpdater* (*)(const std::string& scenarioFile, bool forceReload);

void SetScenarioFactory(ScenarioFactoryFn fn);
IScenarioUpdater* CreateScenario(const std::string& scenarioFile, bool forceReload);
} // namespace ECSEngine
