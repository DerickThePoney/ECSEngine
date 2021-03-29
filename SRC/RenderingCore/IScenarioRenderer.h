#pragma once

namespace ECSEngine
{
class IScenarioRenderer
{
public:
    virtual void Initialise() = 0;
    virtual void Shutdown() = 0;
    virtual void Render() = 0;
};
} // namespace ECSEngine
