#pragma once
#include "ECSCore/UIController.h"

namespace ECSEngine
{
namespace UI
{
class RocketUIController : public UIController
{
protected:
    void VirtualUpdate() override;

public:
    void DrawOrbit(Rendering::DrawCommandBuffer* buffer, const Rendering::MaterialInstanceHandle& material) const;

private:
    std::vector<vec3> FOrbitPoints;
    vec3 FPeriapsis = vec3(0.f);
    vec3 FApoapsis = vec3(0.f);
    bool FHasPeriapsis = false;
    bool FHasApoapsis = false;
    bool FDrawPredictedPath = true;
};
} // namespace UI
} // namespace ECSEngine