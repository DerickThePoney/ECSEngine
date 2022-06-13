#pragma once

#include "ECSCore/UIController.h"

namespace ECSEngine
{
namespace UI
{
class ColonySelectionPanelController : public UIController
{
public:
    ColonySelectionPanelController();
    ~ColonySelectionPanelController();

protected:
    void VirtualUpdate() override;

private:
};
} // namespace UI
} // namespace ECSEngine
