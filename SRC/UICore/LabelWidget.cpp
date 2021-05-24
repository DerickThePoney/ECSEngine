#include "stdafx.h"

#include "LabelWidget.h"

#include "Common/Resource.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/UICommandBuffer.h"

namespace ECSEngine
{
namespace UI
{

WidgetPtr LabelWidgetDescriptor::CreateThisWidget() const
{
    return nullptr;
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

SimpleLabel::SimpleLabel(const std::string& parText, const u32 parColor, const std::string& parFontName, const float parFontSize, const WidgetPlacement& parPlacement)
    : Widget(parPlacement)
    , FText(parText)
    , FTextColor(parColor)
{
    Resource r(parFontName);

    FFont.SetFontSize(parFontSize);
    FFont.InitFromResource(r);

    FMaterial = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\uisimpletextmaterial.material");
}

void SimpleLabel::VirtualOnDraw(Rendering::UICommandBuffer& parBuffer)
{
    Widget::VirtualOnDraw(parBuffer);

    Rendering::VertexDataStream* stream = nullptr;
    std::vector<u32>* indices = nullptr;
    parBuffer.GetCurrentStreams(FMaterial, stream, indices);
}

} // namespace UI
} // namespace ECSEngine