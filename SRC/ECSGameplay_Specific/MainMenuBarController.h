#pragma once
#include "ColonyModule.h"
#include "ColonyPeonsManagementModule.h"
#include "ECSCore/UIController.h"
#include "ECSGameplay_Common/WorldIds.h"
#include "EnergyProducerModule.h"
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

class MainMenuBarController : public UIControllerWithModuleAccessors<MC<ColonyModule, EEntityWorlds::COLONY>,
                                    MC<ResourceStorageModule, EEntityWorlds::COLONY>,
                                    MC<PeonSpawnModule, EEntityWorlds::COLONY>,
                                    MC<ColonyPeonsManagementModule, EEntityWorlds::COLONY>,
                                    MC<PeonFeedingTimeModule, EEntityWorlds::COLONY>,
                                    MC<HousingPlaceModule, EEntityWorlds::BUILDINGS>,
                                    MC<EnergyProducerModule, EEntityWorlds::BUILDINGS>>
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