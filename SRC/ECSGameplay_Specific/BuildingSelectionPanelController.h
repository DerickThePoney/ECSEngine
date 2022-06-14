#pragma once
#include "ECSCore/EntityId.h"
#include "ECSCore/UIController.h"
#include "GameResources.h"
#include "UICore/RML_fwd.h"

namespace ECSEngine
{
namespace UI
{
class IDataModelWrapper;
class BuildingSelectionPanelController : public UIController
{
public:
    BuildingSelectionPanelController();
    ~BuildingSelectionPanelController();

protected:
    void VirtualInit() override;
    void VirtualUpdate() override;
    void VirtualDestroy() override;

private:
    bool HandleVisibility();

private:
    EntityId FCurrentIdSelected;
    EntityId FPreviousIdSelected;

    std::unique_ptr<IDataModelWrapper> FDataModelWrapper;
    Rml::ElementDocument* FDocument = nullptr;
};

} // namespace UI
} // namespace ECSEngine