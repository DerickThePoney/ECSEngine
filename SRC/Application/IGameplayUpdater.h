#pragma once

namespace ECSEngine
{
class IGameplayUpdater
{
public:
    virtual void Initialise() = 0;
    virtual void Shutdown() = 0;
    virtual bool CheckShouldFinish() = 0;
    virtual void StartUpdate() = 0;
    virtual void GameplayUpdate() = 0;
    virtual void UIUpdate() = 0;
    virtual void DebugRender() = 0;
    virtual void Render() = 0;
    virtual void EndUpdate() = 0;
};
} // namespace ECSEngine
