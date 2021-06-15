#include "stdafx.h"

#include "PanelWidgets.h"

#include "Application/PropertyDrawer.h"
#include "Common/ColorUtils.h"
#include "Common/MeshStreamingData.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/UICommandBuffer.h"

namespace ECSEngine
{
namespace UI
{

WidgetPtr PanelWidgetDescriptor::CreateThisWidget() const
{
    return WidgetPtr(new SimplePanel(this, InitialPlacement()));
}

bool PanelWidgetDescriptor::VirtualDrawEditor()
{
    if (ImGui::CollapsingHeader("Panel widget"))
    {
        WidgetDescriptor::VirtualDrawEditor();
        ImGui::Separator();

        glm::vec4 color = ColorUtils::ConvertToFVEC4(FColor);
        EDITOR_PROPERTY_COLOR("Panel color", color);
        FColor = ColorUtils::ConvertToU32(color);
        return true;
    }
    return false;
}

IMPLEMENT_POOL_ALLOCATED(SimplePanel);
SimplePanel::SimplePanel(const PanelWidgetDescriptor* parDescriptor, const WidgetPlacement& parPlacement)
    : Widget(parPlacement)
    , FDescriptor(parDescriptor)
{
    FMaterialInstanceHandle = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\uivertexcolormaterial.material");
}

void SimplePanel::VirtualOnDraw(Rendering::UICommandBuffer& parBuffer)
{
    Widget::VirtualOnDraw(parBuffer);

    Rendering::VertexDataStream* stream = nullptr;
    std::vector<u32>* indices = nullptr;

    parBuffer.GetCurrentStreams(FMaterialInstanceHandle, stream, indices);

    // TODO DRAWER FOR ADDING SPECIFIC STUFF IN THERE FOR FACTORIZATION
    const UI::WidgetPlacement& placement = Placement();
    glm::vec2 pos = glm::xy(placement.FPositionInPixels);
    glm::vec2 size = glm::zw(placement.FPositionInPixels);
    u32 cl = FDescriptor->Color();
    const u32 indexOffset = (u32)stream->GetSize();

    stream->PushData(Rendering::VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(pos, 0.f));
    stream->PushData(Rendering::VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, cl);
    stream->Advance();

    stream->PushData(Rendering::VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(pos + glm::vec2(size.x, 0.f), 0.f));
    stream->PushData(Rendering::VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, cl);
    stream->Advance();

    stream->PushData(Rendering::VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(pos + size, 0.f));
    stream->PushData(Rendering::VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, cl);
    stream->Advance();

    stream->PushData(Rendering::VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(pos + glm::vec2(0.f, size.y), 0.f));
    stream->PushData(Rendering::VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, cl);
    stream->Advance();

    indices->push_back(indexOffset + 0);
    indices->push_back(indexOffset + 1);
    indices->push_back(indexOffset + 2);

    indices->push_back(indexOffset + 0);
    indices->push_back(indexOffset + 2);
    indices->push_back(indexOffset + 3);

    parBuffer.PushContext(placement.FPositionInPixels);
    foreachitem(child, Children()) { child->Draw(parBuffer); }
    parBuffer.PopContext();
}

} // namespace UI
} // namespace ECSEngine