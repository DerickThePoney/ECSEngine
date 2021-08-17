#pragma once
#include "ColonyModule.h"
#include "ColonyPeonsManagementModule.h"
#include "ECSCore/UIController.h"
#include "ECSCore/WorldIds.h"
#include "HousingPlaceModule.h"
#include "PeonFeedingTimeModule.h"
#include "PeonSpawnModule.h"
#include "ResourceStorageModule.h"
#include "UICore/RML_fwd.h"
#include "UICore/RmlDataModelWrapper.h"

namespace ECSEngine
{
namespace UI
{

class MainMenuBarController : public UIControllerWithModuleAccessors<MC<ColonyModule, Worlds::COLONY>,
                                    MC<ResourceStorageModule, Worlds::COLONY>,
                                    MC<PeonSpawnModule, Worlds::COLONY>,
                                    MC<ColonyPeonsManagementModule, Worlds::COLONY>,
                                    MC<PeonFeedingTimeModule, Worlds::COLONY>,
                                    MC<HousingPlaceModule, Worlds::BUILDINGS>>
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
    struct MainMenuBarModel
    {
        u32 TotalPeons = 0;
        u32 IdlePeons = 0;
        int RemainingFeedingTime = 0;
    };

    MainMenuBarModel FModel;
    std::unique_ptr<IDataModelWrapper> FDataModelWrapper;

    Rml::ElementDocument* FDocument = nullptr;
};
} // namespace UI
} // namespace ECSEngine