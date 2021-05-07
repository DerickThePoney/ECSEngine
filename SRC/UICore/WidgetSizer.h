#pragma once

namespace ECSEngine
{
namespace UI
{

namespace PositionningType
{
enum Type
{
    SCREEN_RELATIVE,
    PIXEL,
    LENGTH
};

const char* AsString(PositionningType::Type parValue);
} // namespace PositionningType

namespace ParentAnchor
{
enum Type
{
    TOP_LEFT,
    TOP_CENTER,
    TOP_RIGHT,
    CENTER_LEFT,
    CENTER_CENTER,
    CENTER_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_CENTER,
    BOTTOM_RIGHT,
    LENGTH
};

const char* AsString(ParentAnchor::Type parValue);
} // namespace ParentAnchor

class WidgetScaler;
struct WidgetPlacement
{
    void UpdatePlacementIFN(const WidgetScaler* parScaler);

    PositionningType::Type FPositioningType = PositionningType::PIXEL;
    ParentAnchor::Type FParentAnchor = ParentAnchor::TOP_LEFT;

    glm::vec2 FPositionFromAnchor = glm::vec2(0.f, 0.f);
    glm::vec2 FSize = glm::vec2(1.f, 1.f);
    glm::vec4 FPositionInPixels = glm::vec4(0.f, 0.f, 0.f, 0.f);

    bool FIsDirty = true;
};
} // namespace UI
} // namespace ECSEngine
