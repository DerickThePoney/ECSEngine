#pragma once
#include "Application/IGameplayUpdater.h"

namespace ECSEngine
{
class ApplicationUpdater;

class ApplicationUpdaterWrapper final : public IGameplayUpdater
{
public:
    ApplicationUpdaterWrapper();
    ~ApplicationUpdaterWrapper();

    void Initialise() override;

    void Shutdown() override;

    bool CheckShouldFinish() override;

    void StartUpdate() override;

    void GameplayUpdate() override;

    void UIUpdate() override;

    void DebugRender() override;

    void Render() override;

    void EndUpdate() override;

    template<typename Archive>
    void serialize(Archive& ar)
    {
    }

private:
    ApplicationUpdater* FWrappedGameplayUpdater = nullptr;
};
} // namespace ECSEngine
