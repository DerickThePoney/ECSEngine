#pragma once
#include "Common/MeshStreamingData.h"
#include "Common/RenderingHandles.h"

namespace ECSEngine
{
namespace Rendering
{

class UICommandBuffer
{
public:
    void Clear();

    void PushContext(const glm::vec4 parRectSize);
    void PopContext();

    void Submit();

    void GetCurrentStreams(const MaterialInstanceHandle parMaterialHandle, VertexDataStream*& outVertexStream, std::vector<u32>*& outIndexStream);
    void GetCurrentStreams(const MaterialInstanceHandle parMaterialHandle,
          const TextureHandle parTextureHandle,
          VertexDataStream*& outVertexStream,
          std::vector<u32>*& outIndexStream);

private:
    struct UIBufferContextTriangleBag
    {
        VertexDataStream FStream;
        std::vector<u32> FIndices;
        TextureHandle FTextureHandle;
    };

    struct UIBufferContext
    {
        glm::vec4 RectSize;
        std::map<MaterialInstanceHandle, std::vector<UIBufferContextTriangleBag>> BufferContextTriangleBags;
    };

private:
    u32 CurrentContext() const { return FCurrentContexts.top(); }
    void InitTriBag(const MaterialInstanceHandle parMaterialHandle, const TextureHandle parTextureHandle, UIBufferContextTriangleBag& initTriBag);

private:
    std::vector<UIBufferContext> FBufferContexts;
    std::stack<u32> FCurrentContexts;
};
} // namespace Rendering
} // namespace ECSEngine
