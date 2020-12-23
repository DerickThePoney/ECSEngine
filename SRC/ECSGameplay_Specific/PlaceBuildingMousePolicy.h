#pragma once
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
    void SetupMousePolicy(const std::string& parBuildingTemplateName);

protected:
    void VirtualActivate() override;
    void VirtualDeactivate() override;
    void VirtualUpdate() override;

private:
    glm::vec3 GetMouseWorldPosition() const;

private:
    const EntityTemplate* FTemplate = nullptr;
    Rendering::GFXRepresentationProxy* FBuildingProxy = nullptr;
};
} // namespace ECSEngine