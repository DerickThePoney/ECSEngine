#include "stdafx.h"

#include "Material.h"

#include "Application/PropertyDrawer.h"
#include "BGFXRenderingUtils.h"
#include "MaterialManager.h"
#include "TexturesManager.h"
#include "MaterialDescriptors.h"

namespace ECSEngine
{
namespace Rendering
{
//----------------------------------------------------------------
//          Program
//----------------------------------------------------------------
Program::Program(const ProgramDescriptor* const parDescriptor)
    : RefCountedObject()
    , FDescriptor(parDescriptor)
{
    AssertRelease(parDescriptor != nullptr);
    FHandle = LoadProgram(parDescriptor->GetShadersBasePath(), parDescriptor->GetShadersBaseName());
    AssertRelease(IsValid());
}

Program::~Program()
{
    bgfx::destroy(FHandle);
}

bool Program::IsValid() const
{
    return bgfx::isValid(FHandle);
}

//----------------------------------------------------------------
//          MultipassProgram
//----------------------------------------------------------------
MultiPassProgram::MultiPassProgram(const MultiPassProgramDescriptor* const parDescriptor)
    : RefCountedObject()
    , FDescriptor(parDescriptor)
{
    AssertRelease(parDescriptor != nullptr);
    InitialisePrograms();
    AssertRelease(IsValid());
}

MultiPassProgram::~MultiPassProgram()
{
    foreachitem(programs, FHandles) { bgfx::destroy(programs.second); }
}

const bgfx::ProgramHandle& MultiPassProgram::ProgramHandle(const RenderPassId::Type parRenderPass) const
{
    auto prog = FHandles.find(parRenderPass);
    AssertRelease(prog != FHandles.end());
    AssertRelease(bgfx::isValid(prog->second));
    return prog->second;
}

bool MultiPassProgram::HasSubstitution(const RenderPassId::Type parRenderPass) const
{
    return FHandles.find(parRenderPass) != FHandles.end();
}

bool MultiPassProgram::IsValid() const
{
    foreachitemconst(programs, FHandles)
    {
        if (!bgfx::isValid(programs.second))
            return false;
    }
    return true;
}

void MultiPassProgram::InitialisePrograms()
{
    auto substitutions = FDescriptor->RenderPassToShaders();
    auto defaultSubs = substitutions.find(FDescriptor->DefaultSubstitution());
    AssertRelease(defaultSubs != substitutions.end());
    foreachitemconst(subs, substitutions)
    {
        const std::string& vertexShader = (subs.second[0].empty()) ? defaultSubs->second[0] : subs.second[0];
        const std::string& fragmentShader = (subs.second[1].empty()) ? defaultSubs->second[1] : subs.second[1];
        AssertRelease(!vertexShader.empty());
        AssertRelease(!fragmentShader.empty());
        bgfx::ProgramHandle prog = LoadProgram({ vertexShader, fragmentShader });
        FHandles.insert_or_assign(subs.first, prog);
    }
}

//----------------------------------------------------------------
//          MaterialInstanceDescriptor
//----------------------------------------------------------------
IMPLEMENT_POOL_ALLOCATED(MaterialInstance);

MaterialInstance::MaterialInstance(const Program* const parProgram, const MaterialDescriptor* const parMaterialDescriptor)
    : RefCountedObject()
    , FProgram(parProgram)
    , FMaterialDescriptor(parMaterialDescriptor)
{
    AssertRelease(FProgram != nullptr);
    AssertRelease(FProgram->IsValid());
    AssertRelease(FMaterialDescriptor != nullptr);

    const std::vector<MaterialTextureInputDescriptor>& texturesInputDesc = FMaterialDescriptor->GetTexturesInput();
    foreachitemconst(desc, texturesInputDesc)
    {
        const TextureHandle handle = TextureManager::Instance().GetTextureHandle(desc.GetTexture());
        AssertRelease(handle.IsValid());
        FTextureInput.push_back(MaterialTextureInput(handle, desc.GetTextureSlotName(), desc.GetSlot()));
    }
}

MaterialInstance::~MaterialInstance()
{
}

void MaterialInstance::SetSamplerUniform(const std::string& parUniformName, const TextureHandle& parHandle, const u32 parSlot)
{
#ifdef PERFORM_SECURITY_CHECKS
    AlwaysCheckedAssert(FProgram->Descriptor()->UsesUniformOfType(parUniformName, bgfx::UniformType::Sampler));
#endif
    MaterialManager::SetSamplerUniform(parUniformName, parHandle, parSlot);
}

void MaterialInstance::SetVec4Uniform(const std::string& parUniformName, const glm::vec4& parUniformValue)
{
#ifdef PERFORM_SECURITY_CHECKS
    AlwaysCheckedAssert(FProgram->Descriptor()->UsesUniformOfType(parUniformName, bgfx::UniformType::Vec4));
#endif
    MaterialManager::SetVec4Uniform(parUniformName, parUniformValue);
}

void MaterialInstance::SetMat3Uniform(const std::string& parUniformName, const glm::mat3& parUniformValue)
{
#ifdef PERFORM_SECURITY_CHECKS
    AlwaysCheckedAssert(FProgram->Descriptor()->UsesUniformOfType(parUniformName, bgfx::UniformType::Mat3));
#endif
    MaterialManager::SetMat3Uniform(parUniformName, parUniformValue);
}

void MaterialInstance::SetMat4Uniform(const std::string& parUniformName, const glm::mat4& parUniformValue)
{
#ifdef PERFORM_SECURITY_CHECKS
    AlwaysCheckedAssert(FProgram->Descriptor()->UsesUniformOfType(parUniformName, bgfx::UniformType::Mat4));
#endif
    MaterialManager::SetMat4Uniform(parUniformName, parUniformValue);
}

void MaterialInstance::SetTextures() const
{
    foreachitemconst(textureInput, FTextureInput) { textureInput.SetTexture(); }
}

void MaterialInstance::SetFreeFormSamplerUniform(const std::string& parUniformName, const u32& parHandle, const u32 parSlot) const
{
#ifdef PERFORM_SECURITY_CHECKS
    AlwaysCheckedAssert(FProgram->Descriptor()->UsesUniformOfType(parUniformName, bgfx::UniformType::Sampler));
#endif
    MaterialManager::SetFreeFormSamplerUniform(parUniformName, parHandle, parSlot);
}

} // namespace Rendering
} // namespace ECSEngine
