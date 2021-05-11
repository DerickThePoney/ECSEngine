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

void UICommandBuffer::PushContext(const glm::vec4 parRectSize)
{
    AssertRelease(!FCurrentContexts.empty() || FBufferContexts.size() == 0);

    UIBufferContext newContext;
    newContext.RectSize = parRectSize;
    FBufferContexts.push_back(newContext);
    FCurrentContexts.push((u32)FCurrentContexts.size());
}

void UICommandBuffer::PopContext()
{
    AssertRelease(!FCurrentContexts.empty() || FBufferContexts.size() == 0);
    if (FCurrentContexts.empty())
        return;

    FCurrentContexts.pop();
}

void UICommandBuffer::Submit()
{
    AlwaysCheckedAssert(FCurrentContexts.empty());

    RenderingState state(0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));
    foreachitemconst(context, FBufferContexts)
    {
        bgfx::setScissor(context.RectSize.x, context.RectSize.y, context.RectSize.z, context.RectSize.w);
        foreachitemconst(bufferStream, context.BufferContextTriangleBags)
        {
            auto itFind = FBufferBags.find(bufferStream.first);
            if (itFind == FBufferBags.end())
            {
                itFind = FBufferBags.insert_or_assign(bufferStream.first, UICommandBufferBufferBag()).first;
                itFind->second.VertexBuffer.SetHash(bufferStream.second.FStream.GetHash());
            }

            itFind->second.VertexBuffer.PushRawBuffer(bufferStream.second.FStream);
            itFind->second.IndexBuffer.PushData(bufferStream.second.FIndices.data(), (u32)bufferStream.second.FIndices.size());

            state.ApplyState();

            bgfx::setVertexBuffer(0, itFind->second.VertexBuffer.GetVertexBufferHandle(), 0u, itFind->second.VertexBuffer.GetNumberOfVertices(),
                  itFind->second.VertexBuffer.GetVertexLayoutHandle());
            bgfx::setIndexBuffer(itFind->second.IndexBuffer.GetIndexBufferHandle(), 0u, itFind->second.IndexBuffer.GetSize());

            const Rendering::MaterialInstance* instance = Rendering::MaterialManager::GetMaterialInstance(bufferStream.first);
            AssertRelease(instance != nullptr);
            bgfx::submit(RenderPassId::GAME_UI_PASS, instance->GetProgram()->ProgramHandle());
        }
    }
}

void UICommandBuffer::GetCurrentStreams(const MaterialInstanceHandle parMaterialHandle, VertexDataStream* outVertexStream, std::vector<u32>* outIndexStream)
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