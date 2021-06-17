#include "stdafx.h"

#include "UIDrawer.h"

#include "RenderingCore/UICommandBuffer.h"
namespace ECSEngine
{
namespace UI
{

void UIBackgroundDrawer::Draw(Rendering::UICommandBuffer& parBuffer, const glm::vec2 parPosition, const glm::vec2 parSize)
{
    Rendering::VertexDataStream* stream = nullptr;
    std::vector<u32>* indices = nullptr;

    parBuffer.GetCurrentStreams(FMaterialInstanceHandle, stream, indices);

    const u32 indexOffset = (u32)stream->GetSize();

    stream->PushData(Rendering::VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(parPosition, 0.f));
    stream->PushData(Rendering::VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream->Advance();

    stream->PushData(Rendering::VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(parPosition + glm::vec2(parSize.x, 0.f), 0.f));
    stream->PushData(Rendering::VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream->Advance();

    stream->PushData(Rendering::VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(parPosition + parSize, 0.f));
    stream->PushData(Rendering::VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream->Advance();

    stream->PushData(Rendering::VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(parPosition + glm::vec2(0.f, parSize.y), 0.f));
    stream->PushData(Rendering::VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream->Advance();

    indices->push_back(indexOffset + 0);
    indices->push_back(indexOffset + 1);
    indices->push_back(indexOffset + 2);

    indices->push_back(indexOffset + 0);
    indices->push_back(indexOffset + 2);
    indices->push_back(indexOffset + 3);
}

} // namespace UI
} // namespace ECSEngine