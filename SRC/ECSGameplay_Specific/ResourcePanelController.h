#pragma once
#include "ECSCore/UIController.h"
#include "UICore/RML_fwd.h"
#include "UIResourceView.h"

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
    std::map<GameResource::Type, u32> FResourceToIndex;
    std::vector<UIResourceView> FResourceView;

    std::unique_ptr<IDataModelWrapper> FDataModelWrapper;
    Rml::ElementDocument* FDocument = nullptr;
};
} // namespace UI
} // namespace ECSEngine
