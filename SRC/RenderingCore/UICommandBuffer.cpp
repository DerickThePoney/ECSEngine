#include "stdafx.h"

#include "UICommandBuffer.h"

#include "IndexBuffer.h"
#include "Material.h"
#include "MaterialManager.h"
#include "RenderPass.h"
#include "RenderingState.h"
#include "VertexBuffer.h"

namespace ECSEngine
{
namespace Rendering
{

void UICommandBuffer::Clear()
{
    // todo optimise this for reuse
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
        foreachitem(bufferStreams, context.BufferContextTriangleBags)
        {
            const Rendering::MaterialInstance* instance = Rendering::MaterialManager::GetMaterialInstance(bufferStreams.first);
            AssertRelease(instance != nullptr);

            foreachitem(bufferStream, bufferStreams.second)
            {
                DynamicVertexBuffer vertexBuffer;
                DynamicIndexBuffer indexBuffer;

                vertexBuffer.SetHash(bufferStream.FStream.GetHash());

                vertexBuffer.PushRawBuffer(bufferStream.FStream);
                indexBuffer.PushData(bufferStream.FIndices.data(), (u32)bufferStream.FIndices.size());

                if (bufferStream.FTextureHandle.IsValid())
                {
                    auto& slot = instance->GetMaterialDescriptor()->GetTexturesInput()[0];
                    MaterialManager::SetSamplerUniform(slot.GetTextureSlotName(), bufferStream.FTextureHandle, slot.GetSlot());
                }

                state.ApplyState();

                bgfx::setVertexBuffer(0, vertexBuffer.GetVertexBufferHandle(), 0u, vertexBuffer.GetNumberOfVertices(), vertexBuffer.GetVertexLayoutHandle());
                bgfx::setIndexBuffer(indexBuffer.GetIndexBufferHandle(), 0u, indexBuffer.GetSize());

                bgfx::submit(RenderPassId::GAME_UI_PASS, instance->GetProgram()->ProgramHandle());
            }
        }
    }
}

void UICommandBuffer::GetCurrentStreams(const MaterialInstanceHandle parMaterialHandle, VertexDataStream*& outVertexStream, std::vector<u32>*& outIndexStream)
{
    GetCurrentStreams(parMaterialHandle, TextureHandle(), outVertexStream, outIndexStream);
}

void UICommandBuffer::GetCurrentStreams(const MaterialInstanceHandle parMaterialHandle,
      const TextureHandle parTextureHandle,
      VertexDataStream*& outVertexStream,
      std::vector<u32>*& outIndexStream)
{
    const u32 currentContextIdx = CurrentContext();

    AssertRelease(currentContextIdx < (u32)FBufferContexts.size());
    UIBufferContext& currentContext = FBufferContexts[currentContextIdx];

    auto itFind = currentContext.BufferContextTriangleBags.find(parMaterialHandle);
    std::vector<UIBufferContextTriangleBag>::iterator foundIt;
    if (itFind == currentContext.BufferContextTriangleBags.end())
    {
        UIBufferContextTriangleBag newTriBags;
        InitTriBag(parMaterialHandle, parTextureHandle, newTriBags);
        std::vector<UIBufferContextTriangleBag> triBags = { newTriBags };
        itFind = currentContext.BufferContextTriangleBags.insert_or_assign(parMaterialHandle, triBags).first;
        foundIt = itFind->second.begin();
    }
    else
    {
        foundIt = std::find_if(
              itFind->second.begin(), itFind->second.end(), [parTextureHandle](const UIBufferContextTriangleBag& val) { return val.FTextureHandle == parTextureHandle; });

        if (foundIt == itFind->second.end())
        {
            UIBufferContextTriangleBag newTriBags;
            InitTriBag(parMaterialHandle, parTextureHandle, newTriBags);
            itFind->second.push_back(newTriBags);
            foundIt = itFind->second.begin() + itFind->second.size();
        }

#ifdef ENABLE_SECURITY_CHECKS
        const MaterialInstance* instance = MaterialManager::GetMaterialInstance(parMaterialHandle);
        AssertRelease(instance != nullptr);
        const Program* program = instance->GetProgram();
        AssertRelease(program != nullptr);
        VertexLayoutHash vertexHash(program->Descriptor()->LayoutDescription());
        AssertRelease(vertexHash == foundIt->FStream.GetHash());
#endif
    }

    outVertexStream = &foundIt->FStream;
    outIndexStream = &foundIt->FIndices;
}

void UICommandBuffer::InitTriBag(const MaterialInstanceHandle parMaterialHandle, const TextureHandle parTextureHandle, UIBufferContextTriangleBag& initTriBag)
{
    const MaterialInstance* instance = MaterialManager::GetMaterialInstance(parMaterialHandle);
    AssertRelease(instance != nullptr);
    const Program* program = instance->GetProgram();
    AssertRelease(program != nullptr);

    VertexLayoutHash vertexHash(program->Descriptor()->LayoutDescription());

    // ouais c'est moche. Sue me
    VertexDataStream str(16, vertexHash.GetByteSize(), vertexHash, true);
    initTriBag.FStream = str;
    initTriBag.FTextureHandle = parTextureHandle;
}

} // namespace Rendering
} // namespace ECSEngine