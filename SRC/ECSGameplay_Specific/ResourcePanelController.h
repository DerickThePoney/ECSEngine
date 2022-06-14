#pragma once
#include "ECSCore/UIController.h"
#include "GameResources.h"
#include "UICore/RML_fwd.h"

namespace ECSEngine
{
namespace UI
{
class IDataModelWrapper;
class ResourcePanelController : public UIController
{
public:
    ResourcePanelController();
    ~ResourcePanelController();

    void SetIsPlacingBuilding(const u32 parBuildingIndex);

protected:
    void VirtualInit() override;
    void VirtualUpdate() override;
    void VirtualDestroy() override;

    bool HandleVisibility();

private:
    void FillWindow();

private:
    struct ResourceView
    {
        GameResource::Type Resource = GameResource::LENGTH;
        std::string ResourceName;
        int Quantity = 0;
    };

    std::map<GameResource::Type, u32> FResourceToIndex;
    std::vector<ResourceView> FResourceView;

    std::unique_ptr<IDataModelWrapper> FDataModelWrapper;
    Rml::ElementDocument* FDocument = nullptr;
};
} // namespace UI
} // namespace ECSEngine
