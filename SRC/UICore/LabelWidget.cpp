#include "stdafx.h"

#include "LabelWidget.h"

#include "Common/Resource.h"
#include "FontManager.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/UICommandBuffer.h"

namespace ECSEngine
{
namespace UI
{

WidgetPtr LabelWidgetDescriptor::CreateThisWidget() const
{
    return WidgetPtr(new SimpleLabel(FTextToDraw, FTextColor, FFontName, FFontSize, InitialPlacement()));
}

bool LabelWidgetDescriptor::VirtualDrawEditor()
{
    if (ImGui::CollapsingHeader("Label widget"))
    {
        WidgetDescriptor::VirtualDrawEditor();
        return true;
    }
    return false;
}

IMPLEMENT_POOL_ALLOCATED(SimpleLabel);
SimpleLabel::SimpleLabel(const std::string& parText, const u32 parColor, const std::string& parFontName, const float parFontSize, const WidgetPlacement& parPlacement)
    : Widget(parPlacement)
    , FText(parText)
    , FTextColor(parColor)
    , FFontSize(parFontSize)
{
    FFont = Fonts::GetFont(parFontName, parFontSize);
    AssertRelease(FFont != nullptr);
    FMaterial = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\uisimpletextmaterial.material");
}

void SimpleLabel::VirtualOnDraw(Rendering::UICommandBuffer& parBuffer)
{
    Widget::VirtualOnDraw(parBuffer);

    Rendering::VertexDataStream* stream = nullptr;
    std::vector<u32>* indices = nullptr;
    parBuffer.GetCurrentStreams(FMaterial, FFont->FontTexture(), stream, indices);

    AssertRelease(stream != nullptr);
    AssertRelease(indices != nullptr);

    WidgetPlacement& placement = Placement();
    FFont->RasterizeText(FText, FFontSize, placement.GetAnchorPositionInPixels(WidgetParentAnchor::TOP_LEFT), *stream, *indices);
}

} // namespace UI
} // namespace ECSEngine