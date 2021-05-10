#include "stdafx.h"

#include "WidgetPlacement.h"

#include "Widget.h"

namespace ECSEngine
{
namespace UI
{
namespace WidgetPositionningType
{

const char* AsString(WidgetPositionningType::Type parValue)
{
    switch (parValue)
    {
    case WidgetPositionningType::SCREEN_RELATIVE:
        return "SCREEN_RELATIVE";
    case WidgetPositionningType::PIXEL:
        return "PIXEL";
    default:
        return "";
    }
}

} // namespace WidgetPositionningType

namespace WidgetParentAnchor
{

const char* AsString(WidgetParentAnchor::Type parValue)
{
    switch (parValue)
    {
    case WidgetParentAnchor::TOP_LEFT:
        return "TOP_LEFT";
    case WidgetParentAnchor::TOP_CENTER:
        return "TOP_CENTER";
    case WidgetParentAnchor::TOP_RIGHT:
        return "TOP_RIGHT";
    case WidgetParentAnchor::CENTER_LEFT:
        return "CENTER_LEFT";
    case WidgetParentAnchor::CENTER_CENTER:
        return "CENTER_CENTER";
    case WidgetParentAnchor::CENTER_RIGHT:
        return "CENTER_RIGHT";
    case WidgetParentAnchor::BOTTOM_LEFT:
        return "BOTTOM_LEFT";
    case WidgetParentAnchor::BOTTOM_CENTER:
        return "BOTTOM_CENTER";
    case WidgetParentAnchor::BOTTOM_RIGHT:
        return "BOTTOM_RIGHT";
    default:
        return "";
    }
}

} // namespace WidgetParentAnchor

namespace WidgetSizeType
{

const char* AsString(WidgetSizeType::Type parValue)
{
    switch (parValue)
    {
    case WidgetSizeType::ABSOLUTE_PIXEL:
        return "TOP_LEFT";
    case WidgetSizeType::ABSOLUTE_RELATIVE:
        return "ABSOLUTE_RELATIVE";
    case WidgetSizeType::FIT_TO_PARENT:
        return "FIT_TO_PARENT";
    case WidgetSizeType::FIT_TO_CHILDREN:
        return "FIT_TO_CHILDREN";
    default:
        return "";
    }
}

} // namespace WidgetSizeType

void WidgetPlacement::UpdatePlacementIFN(const WidgetScaler* parScaler, const Widget* parParent)
{
    FPositionInPixels.z = FSize.x;
    FPositionInPixels.w = FSize.y;
    switch (FPositioningType)
    {
    case WidgetPositionningType::PIXEL:
    {
        glm::vec2 anchorPosition = FPositionFromAnchor;
        if (parParent != nullptr)
        {
            const WidgetPlacement& parentPlacement = parParent->Placement();
            const glm::vec2 parentAnchorPlacement = parentPlacement.GetAnchorPositionInPixels(FParentAnchor);
            anchorPosition += parentAnchorPlacement;
        }
        const glm::vec2 selfAnchorPositionFromTopLeft = FSelfAnchor * glm::zw(FPositionInPixels);

        const glm::vec2 topLeftPositionInPixel = anchorPosition - selfAnchorPositionFromTopLeft;
        FPositionInPixels.x = topLeftPositionInPixel.x;
        FPositionInPixels.y = topLeftPositionInPixel.y;

        break;
    }
    default:
        AssertNotReached();
    }
}

glm::vec2 WidgetPlacement::GetAnchorPositionInPixels(WidgetParentAnchor::Type parAnchorType) const
{
    switch (parAnchorType)
    {
    case WidgetParentAnchor::TOP_LEFT:
        return glm::xy(FPositionInPixels);
    case WidgetParentAnchor::TOP_CENTER:
    {
        const float displacement = 0.5f * (FPositionInPixels.z - FPositionInPixels.x);
        return glm::xy(FPositionInPixels) + glm::vec2(displacement, 0.f);
    }
    case WidgetParentAnchor::TOP_RIGHT:
    {
        const float displacement = (FPositionInPixels.z - FPositionInPixels.x);
        return glm::xy(FPositionInPixels) + glm::vec2(displacement, 0.f);
    }
    case WidgetParentAnchor::CENTER_LEFT:
    {
        const float displacementy = 0.5f * (FPositionInPixels.w - FPositionInPixels.y);
        return glm::xy(FPositionInPixels) + glm::vec2(0.f, displacementy);
    }
    case WidgetParentAnchor::CENTER_CENTER:
    {
        const float displacementx = 0.5f * (FPositionInPixels.z - FPositionInPixels.x);
        const float displacementy = 0.5f * (FPositionInPixels.w - FPositionInPixels.y);
        return glm::xy(FPositionInPixels) + glm::vec2(displacementx, displacementy);
    }
    case WidgetParentAnchor::CENTER_RIGHT:
    {
        const float displacementx = (FPositionInPixels.z - FPositionInPixels.x);
        const float displacementy = 0.5f * (FPositionInPixels.w - FPositionInPixels.y);
        return glm::xy(FPositionInPixels) + glm::vec2(displacementx, displacementy);
    }
    case WidgetParentAnchor::BOTTOM_LEFT:
    {
        const float displacementy = (FPositionInPixels.w - FPositionInPixels.y);
        return glm::xy(FPositionInPixels) + glm::vec2(0.f, displacementy);
    }
    case WidgetParentAnchor::BOTTOM_CENTER:
    {
        const float displacementx = 0.5f * (FPositionInPixels.z - FPositionInPixels.x);
        const float displacementy = (FPositionInPixels.w - FPositionInPixels.y);
        return glm::xy(FPositionInPixels) + glm::vec2(displacementx, displacementy);
    }
    case WidgetParentAnchor::BOTTOM_RIGHT:
    {
        const float displacementx = (FPositionInPixels.z - FPositionInPixels.x);
        const float displacementy = (FPositionInPixels.w - FPositionInPixels.y);
        return glm::xy(FPositionInPixels) + glm::vec2(displacementx, displacementy);
    }
    default:
        AssertNotReached();
        return glm::vec2(-1.f);
    }
}

} // namespace UI
} // namespace ECSEngine
