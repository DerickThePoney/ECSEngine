#pragma once
#include "ECSCore/EntityId.h"
#include "ECSCore/UIController.h"
#include "GameResources.h"
#include "UICore/RML_fwd.h"

namespace ECSEngine
{
class ResourceStorageModule;
class RecipeProductionModule;
class StorageSlotModule;
namespace UI
{
class UIResourceView;
using UIResourceArray = std::vector<UIResourceView>;

struct UIStorageSlotsView
{
    GameResource::Type Resource = GameResource::LENGTH;
    std::string ResourceName;
    u32 Quantity = 0;
    bool IsFree = false;
};

struct BuildingSelectionPanelDataView
{
    bool HasResources = false;
    UIResourceArray FResourcesInCurrentBuilding;
    std::map<GameResource::Type, u32> FResourceToIndex;

    bool HasRecipe = false;
    UIResourceArray FRecipeInputs;
    UIResourceArray FRecipeOutputs;
    float FRecipeDuration = 0.f;

    bool ProducesEnergy = false;
    float EnergyProduced = 0;

    bool HasStorageSlots = false;
    std::vector<UIStorageSlotsView> StorageSlots;
};

class IDataModelWrapper;
class BuildingSelectionPanelController : public UIController
{
    friend struct BuildingSelectionPanelCallbackListener;

public:
    BuildingSelectionPanelController();
    ~BuildingSelectionPanelController();

protected:
    void VirtualInit() override;
    void VirtualUpdate() override;
    void VirtualDestroy() override;

private:
    bool HandleVisibility();
    void ResetDataView();
    void HandleResources(const ResourceStorageModule* storageModule);
    void HandleRecipe(const RecipeProductionModule* storageModule);
    void HandleStorageSlots(const StorageSlotModule* storageModule);

    void OnDeleteButtonClicked();

private:
    EntityId FCurrentIdSelected;
    EntityId FPreviousIdSelected;

    BuildingSelectionPanelDataView FDataView;

    std::unique_ptr<IDataModelWrapper> FDataModelWrapper;
    Rml::ElementDocument* FDocument = nullptr;
};

} // namespace UI
} // namespace ECSEngine