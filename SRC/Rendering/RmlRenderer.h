#pragma once

#include "Common/RenderingHandles.h"

#include <RmlUi/Core.h>

namespace ECSEngine
{
namespace Rendering
{
class VertexDataStream;
class RmlRenderer final : public Rml::RenderInterface
{
public:
    void Initialise();
    void Shutdown();
    void OnPreUpdate();

    // Rml Interface
    void RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture, const Rml::Vector2f& translation) override;
    Rml::CompiledGeometryHandle CompileGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture) override;
    void RenderCompiledGeometry(Rml::CompiledGeometryHandle geometry, const Rml::Vector2f& translation) override;
    void ReleaseCompiledGeometry(Rml::CompiledGeometryHandle geometry) override;
    void EnableScissorRegion(bool enable) override;
    void SetScissorRegion(int x, int y, int width, int height) override;
    bool LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source) override;
    bool GenerateTexture(Rml::TextureHandle& texture_handle, const byte* source, const Rml::Vector2i& source_dimensions) override;
    void ReleaseTexture(Rml::TextureHandle texture) override;
    void SetTransform(const Rml::Matrix4f* transform) override;

private:
    void FillVextexStream(VertexDataStream& parStream, Rml::Vertex* vertices, int num_vertices);

private:
    MaterialInstanceHandle FRenderMaterial;
    MaterialInstanceHandle FRenderMaterialWithTexture;

    std::set<std::pair<MeshHandle, u32>> FCompiledGeometry;

    glm::mat4 FCurrentMatrix = glm::identity<glm::mat4>();
    glm::mat4 FProjMat = glm::identity<glm::mat4>();
    glm::vec4 FScissor = glm::vec4(0.f);
};
} // namespace Rendering
} // namespace ECSEngine