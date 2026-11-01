#pragma once

namespace ECSEngine
{
class IScenarioUpdater
{
public:
    virtual ~IScenarioUpdater() = default;

    virtual void Initialise() = 0;
    virtual void Destroy() = 0;

    virtual void RealtimeUpdate() = 0;
    virtual void GameplayUpdate() = 0;
    virtual void UIUpdate() = 0;
    virtual void DebugRender() = 0;
    virtual void Render() = 0;
    virtual void EndUpdate() = 0;
};
} // namespace ECSEngine
