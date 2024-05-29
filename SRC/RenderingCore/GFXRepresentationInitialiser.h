#pragma once
namespace ECSEngine
{
namespace Rendering
{
struct GFXRepresentationInitialiser
{
    float FCurrentTime = 0.f;

    vec3 FPosition = vec3(0.f);
    quat FOrientation = quat(1.f, 0.f, 0.f, 0.f);

    std::string FRepresentationDescriptor;

    std::pair<bool, bool> FIsSelectable = { false, false };

    bool HasCarier = false;
    bool HasVisuals = false;
};
} // namespace Rendering
} // namespace ECSEngine