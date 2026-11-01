#include "stdafx.h"

#include "ScenarioFactory.h"

#include "IScenarioUpdater.h"

namespace ECSEngine
{
namespace
{
ScenarioFactoryFn GScenarioFactory = nullptr;
}

void SetScenarioFactory(ScenarioFactoryFn fn)
{
    AssertRelease(fn != nullptr);
    GScenarioFactory = fn;
}

IScenarioUpdater* CreateScenario(const std::string& scenarioFile, bool forceReload)
{
    AssertRelease(GScenarioFactory != nullptr);
    return GScenarioFactory(scenarioFile, forceReload);
}
} // namespace ECSEngine
