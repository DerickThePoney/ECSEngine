#include "stdafx.h"

#include "WidgetSizer.h"

namespace ECSEngine
{
namespace UI
{
namespace PositionningType
{

const char* AsString(PositionningType::Type parValue)
{
    switch (parValue)
    {
    case PositionningType::SCREEN_RELATIVE:
        return "SCREEN_RELATIVE";
    case PositionningType::PIXEL:
        return "PIXEL";
    default:
        return "";
    }
}

} // namespace PositionningType

namespace ParentAnchor
{

const char* AsString(ParentAnchor::Type parValue)
{
    switch (parValue)
    {
    case ParentAnchor::TOP_LEFT:
        return "TOP_LEFT";
    case ParentAnchor::TOP_CENTER:
        return "TOP_CENTER";
    case ParentAnchor::TOP_RIGHT:
        return "TOP_RIGHT";
    case ParentAnchor::CENTER_LEFT:
        return "CENTER_LEFT";
    case ParentAnchor::CENTER_CENTER:
        return "CENTER_CENTER";
    case ParentAnchor::CENTER_RIGHT:
        return "CENTER_RIGHT";
    case ParentAnchor::BOTTOM_LEFT:
        return "BOTTOM_LEFT";
    case ParentAnchor::BOTTOM_CENTER:
        return "BOTTOM_CENTER";
    case ParentAnchor::BOTTOM_RIGHT:
        return "BOTTOM_RIGHT";
    default:
        return "";
    }
}

} // namespace ParentAnchor

void WidgetPlacement::UpdatePlacementIFN(const WidgetScaler* parScaler)
{
}

} // namespace UI
} // namespace ECSEngine
