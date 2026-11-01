#pragma once
#include "Common/InputCommands.h"
#include "Common/MousePolicy.h"

namespace ECSEngine
{
namespace Rendering
{
class GFXRepresentationProxy;
}
class EntityTemplate;
class PlaceBuildingMousePolicy : public IMousePolicy
{
    MOUSE_POLICY_HEADER(PlaceBuildingMousePolicy, IMousePolicy, MousePolicyType::PLACE_BUILDING);

public:
    PlaceBuildingMousePolicy();
    void SetupMousePolicy(const std::string& parBuildingTemplateName);

protected:
    void VirtualActivate() override;
    void VirtualDeactivate() override;
    void VirtualUpdate() override;

private:
    vec3 GetMouseWorldPosition() const;

private:
    const EntityTemplate* FTemplate = nullptr;
    Rendering::GFXRepresentationProxy* FBuildingProxy = nullptr;
    u32 FCellOccupancy = 1;

    MouseButtonCommand FValidateInputCommand;
    MouseButtonCommand FShiftedValidationCommand;
};
} // namespace ECSEngine
