#include "stdafx.h"

#include "WidgetPlacement.h"

#include "Application/PropertyDrawer.h"
#include "Widget.h"
#include "WidgetScaler.h"

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
    case WidgetPositionningType::RELATIVE_POS:
        return "RELATIVE_POS";
    case WidgetPositionningType::PIXEL_POS:
        return "PIXEL_POS";
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

void WidgetPlacement::DrawEditor()
{
    if (ImGui::BeginCombo("Parent Anchor", WidgetParentAnchor::AsString(FParentAnchor)))
    {
        forrange(i, 0, WidgetParentAnchor::LENGTH)
        {
            WidgetParentAnchor::Type current = (WidgetParentAnchor::Type)i;
            if (ImGui::Selectable(WidgetParentAnchor::AsString(current), FParentAnchor == current))
            {
                FParentAnchor = current;
            }
        }
    }

    if (ImGui::BeginCombo("Positionning type", WidgetPositionningType::AsString(FPositioningType)))
    {
        forrange(i, 0, WidgetPositionningType::LENGTH)
        {
            WidgetPositionningType::Type current = (WidgetPositionningType::Type)i;
            if (ImGui::Selectable(WidgetPositionningType::AsString(current), FPositioningType == current))
            {
                FPositioningType = current;
            }
        }
    }

    EDITOR_PROPERTY_SIMPLE("Position from parent anchor", FPositionFromAnchor);
    EDITOR_PROPERTY_SIMPLE("Self anchor positon", FSelfAnchor);

    if (ImGui::BeginCombo("Size type", WidgetSizeType::AsString(FSizeType)))
    {
        forrange(i, 0, WidgetSizeType::LENGTH)
        {
            WidgetSizeType::Type current = (WidgetSizeType::Type)i;
            if (ImGui::Selectable(WidgetSizeType::AsString(current), FSizeType == current))
            {
                FSizeType = current;
            }
        }
    }

    EDITOR_PROPERTY_SIMPLE("Size", FSize);
    EDITOR_PROPERTY_SIMPLE("Padding", FPadding);
}

void WidgetPlacement::UpdatePlacementIFN(const WidgetScaler* parScaler, const Widget* parParent)
{
    glm::vec2 positionScale = glm::vec2(1.f);

    if (FPositioningType == WidgetPositionningType::RELATIVE_POS)
    {
        if (parParent == nullptr)
        {
            positionScale = parScaler->GetScale();
        }
        else
        {
            const WidgetPlacement& parentPlacement = parParent->Placement();
            positionScale = glm::zw(parentPlacement.FPositionInPixels);
        }
    }

    glm::vec2 sizeScale = glm::vec2(1.f);
    if (FSizeType == WidgetSizeType::ABSOLUTE_RELATIVE)
    {
        if (parParent == nullptr)
        {
            sizeScale = parScaler->GetScale();
        }
        else
        {
            const WidgetPlacement& parentPlacement = parParent->Placement();
            sizeScale = glm::zw(parentPlacement.FPositionInPixels);
        }
    }

    FPositionInPixels.z = sizeScale.x * FSize.x;
    FPositionInPixels.w = sizeScale.y * FSize.y;

    glm::vec2 anchorPosition = FPositionFromAnchor * positionScale;
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

    if (FSizeType == WidgetSizeType::FIT_TO_PARENT)
    {
        const WidgetPlacement& parentPlacement = parParent->Placement();
        const glm::vec2 bottomRight = glm::xy(FPositionInPixels) + glm::zw(FPositionInPixels);
        const glm::vec2 parentBottomRight = glm::xy(parentPlacement.FPositionInPixels) + glm::zw(parentPlacement.FPositionInPixels);
        const glm::vec2 clampedBottomRight = glm::min(bottomRight, parentBottomRight);
        const glm::vec2 clampedSize = clampedBottomRight - glm::xy(FPositionInPixels);
        FPositionInPixels.z = clampedSize.x;
        FPositionInPixels.w = clampedSize.y;
    }
    else if (FSizeType == WidgetSizeType::FIT_TO_CHILDREN)
    {
        AssertNotReachedMsg("Not implemented yet !");
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
        const float displacement = 0.5f * FPositionInPixels.z;
        return glm::xy(FPositionInPixels) + glm::vec2(displacement, 0.f);
    }
    case WidgetParentAnchor::TOP_RIGHT:
    {
        const float displacement = FPositionInPixels.z;
        return glm::xy(FPositionInPixels) + glm::vec2(displacement, 0.f);
    }
    case WidgetParentAnchor::CENTER_LEFT:
    {
        const float displacementy = 0.5f * FPositionInPixels.w;
        return glm::xy(FPositionInPixels) + glm::vec2(0.f, displacementy);
    }
    case WidgetParentAnchor::CENTER_CENTER:
    {
        return glm::xy(FPositionInPixels) + 0.5f * glm::zw(FPositionInPixels);
    }
    case WidgetParentAnchor::CENTER_RIGHT:
    {
        const float displacementx = FPositionInPixels.z;
        const float displacementy = 0.5f * FPositionInPixels.w;
        return glm::xy(FPositionInPixels) + glm::vec2(displacementx, displacementy);
    }
    case WidgetParentAnchor::BOTTOM_LEFT:
    {
        return glm::xy(FPositionInPixels) + glm::vec2(0.f, FPositionInPixels.w);
    }
    case WidgetParentAnchor::BOTTOM_CENTER:
    {
        const float displacementx = 0.5f * FPositionInPixels.z;
        const float displacementy = FPositionInPixels.w;
        return glm::xy(FPositionInPixels) + glm::vec2(displacementx, displacementy);
    }
    case WidgetParentAnchor::BOTTOM_RIGHT:
    {
        return glm::xy(FPositionInPixels) + glm::zw(FPositionInPixels);
    }
    default:
        AssertNotReached();
        return glm::vec2(-1.f);
    }
}

} // namespace UI
} // namespace ECSEngine
