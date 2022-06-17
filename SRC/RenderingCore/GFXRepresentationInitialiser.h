#pragma once
namespace ECSEngine
{
namespace Rendering
{
struct GFXRepresentationInitialiser
{
    float FCurrentTime = 0.f;

    glm::vec3 FPosition = glm::vec3(0.f);
    glm::quat FOrientation = glm::quat(1.f, 0.f, 0.f, 0.f);

    std::string FRepresentationDescriptor;

    std::pair<bool, bool> FIsSelectable;

    bool HasCarier = false;
    bool HasVisuals = false;
};
} // namespace Rendering
} // namespace ECSEngine