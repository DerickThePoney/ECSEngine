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

private:
    u32 CurrentContext() const { return FCurrentContexts.top(); }

private:
    struct UIBufferContextTriangleBag
    {
        VertexDataStream FStream;
        std::vector<u32> FIndices;
    };

    struct UIBufferContext
    {
        glm::vec4 RectSize;
        std::map<MaterialInstanceHandle, UIBufferContextTriangleBag> BufferContextTriangleBags;
    };

    std::vector<UIBufferContext> FBufferContexts;
    std::stack<u32> FCurrentContexts;
};
} // namespace Rendering
} // namespace ECSEngine
