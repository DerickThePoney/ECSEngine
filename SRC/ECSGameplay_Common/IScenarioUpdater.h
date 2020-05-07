#pragma once

namespace ECSEngine
{
class IScenarioUpdater
{
public:
    virtual void Initialise() = 0;
    virtual void Destroy() = 0;

    virtual void Update() = 0;
    virtual void Render() = 0;
};
} // namespace ECSEngine
