#pragma once
#include "ECSCore/UIController.h"
#include "ECSGameplay_Common/UIWindowsPositionning.h"
#include "UICore/RML_fwd.h"
#include "UICore/RmlDataModelWrapper.h"

namespace ECSEngine
{
namespace UI
{

class BuildMenuController : public UIController
{
public:
    BuildMenuController();

    void SetIsPlacingBuilding(const u32 parBuildingIndex);

protected:
    void VirtualInit() override;
    void VirtualUpdate() override;
    void VirtualDestroy() override;

    bool HandleVisibility();

private:
    void FillWindow();

private:
    bool FIsPlacingBuilding = false;

    std::unique_ptr<IDataModelWrapper> FDataModelWrapper;
    Rml::ElementDocument* FDocument = nullptr;
};
} // namespace UI
} // namespace ECSEngine
