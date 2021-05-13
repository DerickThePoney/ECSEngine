#pragma once

namespace ECSEngine
{
namespace UI
{

namespace WidgetPositionningType
{
enum Type
{
    RELATIVE_POS,
    PIXEL_POS,
    LENGTH
};

const char* AsString(WidgetPositionningType::Type parValue);
} // namespace WidgetPositionningType

namespace WidgetParentAnchor
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

const char* AsString(WidgetParentAnchor::Type parValue);
} // namespace WidgetParentAnchor

namespace WidgetSizeType
{
enum Type
{
    ABSOLUTE_PIXEL,
    ABSOLUTE_RELATIVE,
    FIT_TO_PARENT,
    FIT_TO_CHILDREN,
    LENGTH
};

const char* AsString(WidgetSizeType::Type parValue);
} // namespace WidgetSizeType

class WidgetScaler;
class Widget;
struct WidgetPlacement
{
    SERIALIZE() { ar(FPositioningType, FParentAnchor, FSizeType, FPositionFromAnchor, FSelfAnchor, FSize, FPadding); }

    void DrawEditor();

    void UpdatePlacementIFN(const WidgetScaler* parScaler, const Widget* parParent);

    glm::vec2 GetAnchorPositionInPixels(WidgetParentAnchor::Type parAnchorType) const;

    WidgetPositionningType::Type FPositioningType = WidgetPositionningType::PIXEL_POS;
    WidgetParentAnchor::Type FParentAnchor = WidgetParentAnchor::TOP_LEFT;
    WidgetSizeType::Type FSizeType = WidgetSizeType::ABSOLUTE_PIXEL;

    glm::vec2 FPositionFromAnchor = glm::vec2(0.f, 0.f);
    glm::vec2 FSelfAnchor = glm::vec2(0.f, 0.f); // Relative
    glm::vec2 FSize = glm::vec2(1.f, 1.f);
    glm::vec4 FPositionInPixels = glm::vec4(0.f, 0.f, 0.f, 0.f);
    glm::vec4 FPadding = glm::vec4(0.f, 0.f, 0.f, 0.f);

    bool FIsDirty = true;
};
} // namespace UI
} // namespace ECSEngine
