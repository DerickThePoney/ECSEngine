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
}

namespace FParentAnchor
{
enum Type
{
    TOP_LEFT,
    TOP_CENTER,
    TOP_RIGTH,
    CENTER_LEFT,
    CENTER_CENTER,
    CENTER_RIGTH,
    BOTTOM_LEFT,
    BOTTOM_CENTER,
    BOTTOM_RIGTH,
};
}

struct WidgetSizer
{

    glm::vec2 FAnchorPosition;
    glm::vec2 FSize;
    glm::vec4 FPositionInPixels;
};
} // namespace UI
} // namespace ECSEngine
