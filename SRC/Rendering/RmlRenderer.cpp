#include "stdafx.h"

#include "RmlRenderer.h"

#include "Common/ColorUtils.h"
#include "Common/MeshStreamingData.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/Material.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/Mesh.h"
#include "RenderingCore/MeshManager.h"
#include "RenderingCore/RenderPass.h"
#include "RenderingCore/RenderingState.h"
#include "RenderingCore/Texture.h"
#include "RenderingCore/TexturesManager.h"
#include "RenderingCore/VertexLayout.h"
#include "bx/bx.h"

namespace ECSEngine
{
namespace Rendering
{
using namespace Rml;

constexpr RenderPassId::Type renderPass = RenderPassId::EDITOR_UI_PASS;

void RmlRenderer::FillVextexStream(VertexDataStream& parStream, Rml::Vertex* vertices, int num_vertices)
{
    forrange(i, 0, num_vertices)
    {
        parStream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(vertices[i].position.x, vertices[i].position.y, 0.f));
        parStream.PushData(
              VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, ColorUtils::FromRGBA(vertices[i].colour.red, vertices[i].colour.green, vertices[i].colour.blue, vertices[i].colour.alpha));
        parStream.PushData(VERTEX_LAYOUT_PARAMS::HAS_UVS, 0, glm::vec2(vertices[i].tex_coord.x, vertices[i].tex_coord.y));
        parStream.Advance();
    }
}

void RmlRenderer::RenderGeometry(Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture, const Vector2f& translation)
{
    VertexLayoutHash hash(true, 1, 1, false, false, false, false);
    bgfx::VertexLayout layout = GetVertexLayout(hash);

    u32 availableVertices = bgfx::getAvailTransientVertexBuffer(num_vertices, layout);
    AssertRelease(availableVertices == num_vertices);
    u32 availableIndices = bgfx::getAvailTransientIndexBuffer(num_indices);
    AssertRelease(availableIndices == num_indices);

    VertexDataStream stream(num_vertices, hash.GetByteSize(), hash);
    FillVextexStream(stream, vertices, num_vertices);

    bgfx::TransientVertexBuffer vertexBuffer;
    bgfx::TransientIndexBuffer indexBuffer;
    bgfx::allocTransientVertexBuffer(&vertexBuffer, num_vertices, layout);
    bgfx::allocTransientIndexBuffer(&indexBuffer, num_indices, true);
    bx::memCopy(vertexBuffer.data, stream.GetData(), stream.GetByteSize());
    bx::memCopy(indexBuffer.data, indices, num_indices * sizeof(u32));

    glm::mat4 mat = glm::translate(glm::vec3(translation.x, translation.y, 0.f)) * FCurrentMatrix;
    bgfx::setTransform(&mat);

    RenderingState state(0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));
    bgfx::setVertexBuffer(0, &vertexBuffer, 0, num_vertices, vertexBuffer.layoutHandle);
    bgfx::setIndexBuffer(&indexBuffer, 0, num_indices);

    const Rendering::MaterialInstance* instance = nullptr;

    if (texture != 0)
    {
        instance = Rendering::MaterialManager::GetMaterialInstance(FRenderMaterialWithTexture);
        AssertRelease(instance != nullptr);

        auto textureSlot = instance->GetMaterialDescriptor()->GetTexturesInput()[0];
        instance->SetFreeFormSamplerUniform(textureSlot.GetTextureSlotName(), texture, textureSlot.GetSlot());
    }
    else
        instance = Rendering::MaterialManager::GetMaterialInstance(FRenderMaterial);
    AssertRelease(instance != nullptr);

    bgfx::submit(renderPass, instance->GetProgram()->ProgramHandle());
}

Rml::CompiledGeometryHandle RmlRenderer::CompileGeometry(Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture)
{
    VertexLayoutHash hash(true, 1, 1, false, false, false, false);
    bgfx::VertexLayout layout = GetVertexLayout(hash);

    VertexDataStream stream(num_vertices, hash.GetByteSize(), hash);
    FillVextexStream(stream, vertices, num_vertices);

    MeshHandle handle = MeshManager::Instance().CreateMesh(stream, indices, num_indices * sizeof(int));
    std::pair<MeshHandle, u32> p = { handle, (u32)texture };
    AlwaysCheckedAssert(FCompiledGeometry.find(p) == FCompiledGeometry.end());
    FCompiledGeometry.insert(p);
    return (Rml::CompiledGeometryHandle)handle.GetMeshId();
}

void RmlRenderer::RenderCompiledGeometry(Rml::CompiledGeometryHandle geometry, const Vector2f& translation)
{
    glm::mat4 mat = glm::translate(glm::vec3(translation.x, translation.y, 0.f)) * FCurrentMatrix;
    bgfx::setTransform(&mat);

    std::pair<MeshHandle, u32> p = { MeshHandle(), -1 };
    foreachitemconst(cg, FCompiledGeometry)
    {
        if (cg.first == geometry)
        {
            p = cg;
            break;
        }
    }
    AssertRelease(p.second != -1 && p.first.IsValid());

    Mesh* mesh = MeshManager::Instance().GetMesh(p.first);
    AssertRelease(mesh != nullptr);

    bgfx::setVertexBuffer(0, mesh->GetVertexBufferHandle());
    bgfx::setIndexBuffer(mesh->GetIndexBufferHandle());
    RenderingState state(0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));
    state.ApplyState();

    const Rendering::MaterialInstance* instance = nullptr;

    if (p.second != 0)
    {
        instance = Rendering::MaterialManager::GetMaterialInstance(FRenderMaterialWithTexture);
        AssertRelease(instance != nullptr);

        auto textureSlot = instance->GetMaterialDescriptor()->GetTexturesInput()[0];
        instance->SetFreeFormSamplerUniform(textureSlot.GetTextureSlotName(), p.second, textureSlot.GetSlot());
    }
    else
        instance = Rendering::MaterialManager::GetMaterialInstance(FRenderMaterial);

    AssertRelease(instance != nullptr);
    bgfx::submit(Rendering::renderPass, instance->GetProgram()->ProgramHandle());
}

void RmlRenderer::ReleaseCompiledGeometry(Rml::CompiledGeometryHandle geometry)
{
    MeshHandle handle(geometry);
#ifdef ENABLE_SECURITY_CHECKS
    bool found = false;
    foreachitemconst(cg, FCompiledGeometry)
    {
        if (geometry == cg.first)
        {
            found = true;
            break;
        }
    }
    AlwaysCheckedAssert(found);
#endif

    for (auto cg = FCompiledGeometry.begin(); cg != FCompiledGeometry.end(); ++cg)
    {
        if (cg->first == handle)
        {
            FCompiledGeometry.erase(cg);
            break;
        }
    }

    MeshManager::Instance().ReleaseMesh(handle);
}

void RmlRenderer::EnableScissorRegion(bool enable)
{
    glm::vec4 scissor = (enable) ? FScissor : glm::vec4(0.f);
    bgfx::setViewScissor(renderPass, scissor.x, scissor.y, scissor.z, scissor.w);
}

void RmlRenderer::SetScissorRegion(int x, int y, int width, int height)
{
    FScissor = glm::vec4(x, y, width, height);
    bgfx::setViewScissor(renderPass, FScissor.x, FScissor.y, FScissor.z, FScissor.w);
}

bool RmlRenderer::LoadTexture(Rml::TextureHandle& texture_handle, Vector2i& texture_dimensions, const String& source)
{
    const u32 id = TextureManager::Instance().CreateFreeFormTexture(source);
    const Texture* texture = TextureManager::Instance().GetFreeFormTexture(id);
    AlwaysCheckedAssert(texture != nullptr);
    if (texture == nullptr)
        return false;

    texture_handle = id + 1;
    texture_dimensions = Vector2i(texture->Info().width, texture->Info().height);
    return true;
}

bool RmlRenderer::GenerateTexture(Rml::TextureHandle& texture_handle, const byte* source, const Vector2i& source_dimensions)
{
    const u32 id = TextureManager::Instance().CreateFreeFormTexture(source, source_dimensions.x, source_dimensions.y);
    const Texture* texture = TextureManager::Instance().GetFreeFormTexture(id);
    AlwaysCheckedAssert(texture != nullptr);
    if (texture == nullptr)
        return false;

    texture_handle = id + 1;
    return true;
}

void RmlRenderer::ReleaseTexture(Rml::TextureHandle texture)
{
    TextureManager::Instance().ReleaseFreeFormTexture(texture - 1);
}

void RmlRenderer::SetTransform(const Rml::Matrix4f* transform)
{
    if (transform == nullptr)
        FCurrentMatrix = glm::identity<glm::mat4>();

    auto row0 = transform->GetRow(0);
    glm::vec4 row0_glm(row0[0], row0[1], row0[2], row0[3]);
    auto row1 = transform->GetRow(1);
    glm::vec4 row1_glm(row1[0], row1[1], row1[2], row1[3]);
    auto row2 = transform->GetRow(2);
    glm::vec4 row2_glm(row2[0], row2[1], row2[2], row2[3]);
    auto row3 = transform->GetRow(3);
    glm::vec4 row3_glm(row3[0], row3[1], row3[2], row3[3]);

    FCurrentMatrix = glm::mat4(row0_glm, row1_glm, row2_glm, row3_glm);
}

void RmlRenderer::Initialise()
{
    FRenderMaterial = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\uivertexcolormaterial.material");
    FRenderMaterialWithTexture = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\uivertexcolortexcoordmaterial.material");
}

void RmlRenderer::Shutdown()
{
    foreachitemconst(handle, FCompiledGeometry)
    {
        MeshManager::Instance().ReleaseMesh(handle.first);
        TextureManager::Instance().ReleaseFreeFormTexture(handle.second);
    }
    FCompiledGeometry.clear();
}

void RmlRenderer::OnPreUpdate()
{
    SCOPED_PROFILE_CLASS(RmlRenderer, OnPreUpdate);
    bgfx::setViewName(renderPass, "GAME_UI_PASS");
    bgfx::setViewMode(renderPass, bgfx::ViewMode::Sequential);

    auto size = GLFWDisplayWindowHandler::Instance().GetSize();
    FProjMat = glm::ortho(0.f, (float)size.x, (float)size.y, 0.f);
    auto id = glm::identity<glm::mat4>();
    bgfx::setViewTransform(renderPass, &id, &FProjMat);
    bgfx::setViewRect(renderPass, 0, 0, uint16_t(size.x), uint16_t(size.y));
}

} // namespace Rendering
} // namespace ECSEngine
