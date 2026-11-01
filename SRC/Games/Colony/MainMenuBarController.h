#pragma once

#include "ECSCore/UIController.h"
#include "UICore/RML_fwd.h"
#include "UICore/RmlDataModelWrapper.h"

namespace ECSEngine
{
namespace UI
{

class MainMenuBarController : public UIController
{
public:
    MainMenuBarController();
    ~MainMenuBarController();

protected:
    void VirtualInit() override;
    void VirtualUpdate() override;
    void VirtualDestroy() override;

    bool HandleVisibility();

private:
    void UpdateEnergy();

private:
    struct MainMenuBarModel
    {
        u32 TotalPeons = 0;
        u32 IdlePeons = 0;
        u32 Influence = 0;
        i32 Energy = 0;
        int RemainingFeedingTime = 0;
    };

    MainMenuBarModel FModel;
    std::unique_ptr<IDataModelWrapper> FDataModelWrapper;

    Rml::ElementDocument* FDocument = nullptr;
};
} // namespace UI
} // namespace ECSEngine