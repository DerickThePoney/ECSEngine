#include "stdafx.h"

#include "UICommandBuffer.h"

#include "Material.h"
#include "MaterialManager.h"
#include "RenderPass.h"
#include "RenderingState.h"

namespace ECSEngine
{
namespace Rendering
{

void UICommandBuffer::Clear()
{
    while (!FCurrentContexts.empty())
    {
        FCurrentContexts.pop();
    }

    FBufferContexts.clear();
}

void UICommandBuffer::PushContext(const glm::vec4 parRectSize)
{
    UIBufferContext newContext;
    newContext.RectSize = parRectSize;
    FCurrentContexts.push((u32)FBufferContexts.size());
    FBufferContexts.push_back(newContext);
}

void UICommandBuffer::PopContext()
{
    if (FCurrentContexts.empty())
        return;

    FCurrentContexts.pop();
}

void UICommandBuffer::Submit()
{
    AlwaysCheckedAssert(FCurrentContexts.empty());

    RenderingState state(0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));
    foreachitem(context, FBufferContexts)
    {
        bgfx::setScissor(context.RectSize.x, context.RectSize.y, context.RectSize.z, context.RectSize.w);
        foreachitem(bufferStream, context.BufferContextTriangleBags)
        {
            bufferStream.second.FBufferBag.VertexBuffer.PushRawBuffer(bufferStream.second.FStream);
            bufferStream.second.FBufferBag.IndexBuffer.PushData(bufferStream.second.FIndices.data(), (u32)bufferStream.second.FIndices.size());

            state.ApplyState();

            bgfx::setVertexBuffer(0, bufferStream.second.FBufferBag.VertexBuffer.GetVertexBufferHandle(), 0u, bufferStream.second.FBufferBag.VertexBuffer.GetNumberOfVertices(),
                  bufferStream.second.FBufferBag.VertexBuffer.GetVertexLayoutHandle());
            bgfx::setIndexBuffer(bufferStream.second.FBufferBag.IndexBuffer.GetIndexBufferHandle(), 0u, bufferStream.second.FBufferBag.IndexBuffer.GetSize());

            const Rendering::MaterialInstance* instance = Rendering::MaterialManager::GetMaterialInstance(bufferStream.first);
            AssertRelease(instance != nullptr);
            bgfx::submit(RenderPassId::GAME_UI_PASS, instance->GetProgram()->ProgramHandle());
        }
    }
}

void UICommandBuffer::GetCurrentStreams(const MaterialInstanceHandle parMaterialHandle, VertexDataStream*& outVertexStream, std::vector<u32>*& outIndexStream)
{
    const u32 currentContextIdx = CurrentContext();

    AssertRelease(currentContextIdx < (u32)FBufferContexts.size());
    UIBufferContext& currentContext = FBufferContexts[currentContextIdx];

    auto itFind = currentContext.BufferContextTriangleBags.find(parMaterialHandle);
    if (itFind == currentContext.BufferContextTriangleBags.end())
    {
        UIBufferContextTriangleBag newTriBags;
        const MaterialInstance* instance = MaterialManager::GetMaterialInstance(parMaterialHandle);
        AssertRelease(instance != nullptr);
        const Program* program = instance->GetProgram();
        AssertRelease(program != nullptr);

        VertexLayoutHash vertexHash(program->Descriptor()->LayoutDescription());

        // ouais c'est moche. Sue me
        VertexDataStream str(16, vertexHash.GetByteSize(), vertexHash, true);
        newTriBags.FStream = str;

        itFind = currentContext.BufferContextTriangleBags.insert_or_assign(parMaterialHandle, newTriBags).first;
        itFind->second.FBufferBag.VertexBuffer.SetHash(vertexHash);
    }
#ifdef ENABLE_SECURITY_CHECKS
    else
    {
        const MaterialInstance* instance = MaterialManager::GetMaterialInstance(parMaterialHandle);
        AssertRelease(instance != nullptr);
        const Program* program = instance->GetProgram();
        AssertRelease(program != nullptr);
        VertexLayoutHash vertexHash(program->Descriptor()->LayoutDescription());
        AssertRelease(vertexHash == itFind->second.FStream.GetHash());
    }
#endif

    outVertexStream = &itFind->second.FStream;
    outIndexStream = &itFind->second.FIndices;
}

} // namespace Rendering
} // namespace ECSEngine