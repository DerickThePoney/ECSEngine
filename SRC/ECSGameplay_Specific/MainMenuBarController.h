#pragma once
#include "ColonyModule.h"
#include "ColonyPeonsManagementModule.h"
#include "ECSCore/UIController.h"
#include "ECSCore/WorldIds.h"
#include "HousingPlaceModule.h"
#include "PeonFeedingTimeModule.h"
#include "PeonSpawnModule.h"
#include "ResourceStorageModule.h"

namespace Rml
{
class ElementDocument;
}

namespace ECSEngine
{
namespace UI
{

class IDataModelWrapper
{
public:
    virtual bool IsVariableDirty(const std::string& variable_name) = 0;
    virtual void DirtyVariable(const std::string& variable_name) = 0;

    virtual explicit operator bool() = 0;
};

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
    };

    MainMenuBarModel FModel;
    std::unique_ptr<IDataModelWrapper> FDataModelWrapper;

    Rml::ElementDocument* FDocument = nullptr;
};
} // namespace UI
} // namespace ECSEngine