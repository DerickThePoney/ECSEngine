#include "stdafx.h"

#include "Material.h"

#include "Application/PropertyDrawer.h"
#include "BGFXRenderingUtils.h"
#include "MaterialManager.h"
#include "TexturesManager.h"

namespace ECSEngine
{
namespace Rendering
{
//----------------------------------------------------------------
//          ProgramDescriptor
//----------------------------------------------------------------
IMPLEMENT_POOL_ALLOCATED(ProgramDescriptor);
ProgramDescriptor::ProgramDescriptor()
    : RefCountedObject()
{
}

ProgramDescriptor::~ProgramDescriptor()
{
}

#ifdef PERFORM_SECURITY_CHECKS
bool ProgramDescriptor::UsesUniformOfType(const std::string& parName, bgfx::UniformType::Enum parType) const
{
    foreachitemconst(uniform, FUniformsAndTypes)
    {
        if (parName == uniform.first)
        {
            return parType == uniform.second;
        }
    }
    return false;
}
#endif

//----------------------------------------------------------------
//          ProgramDescriptorV2
//----------------------------------------------------------------
IMPLEMENT_POOL_ALLOCATED(ProgramDescriptorV2);
ProgramDescriptorV2::ProgramDescriptorV2()
    : RefCountedObject()
{
}

ProgramDescriptorV2::~ProgramDescriptorV2()
{
}

void ProgramDescriptorV2::DrawEditor()
{
    FDefaultView = RenderPassId::ChooseInList(FDefaultView);
    if (ImGui::CollapsingHeader("Substitutions"))
    {
        static RenderPassId::Type selectedType = FDefaultView;        

        RenderPassId::Type chosenPass = RenderPassId::ChooseInList(selectedType);
        ImGui::SameLine();
        if (ImGui::Button("Add substitution"))
        {
            auto it = FRenderPassToShaderMap.find(chosenPass);
            if (it == FRenderPassToShaderMap.end())
                FRenderPassToShaderMap.insert_or_assign(chosenPass, ShaderSubstitutionList());
        }

        u32 idx = 0;
        foreachitem(substitution, FRenderPassToShaderMap)
        {
            ImGui::Indent();
            ImGui::PushID(idx);
            RenderPassId::Type modifyChosenPass = RenderPassId::ChooseInList(substitution.first);

            ImGui::SameLine();
            const bool deleteSubstitution = ImGui::Button("Delete substitution");

            if (modifyChosenPass != substitution.first)
            {
                auto itFind = FRenderPassToShaderMap.find(modifyChosenPass);
                if (itFind == FRenderPassToShaderMap.end())
                {
                    ShaderSubstitutionList shaderList = substitution.second;
                    FRenderPassToShaderMap.erase(itFind);
                    FRenderPassToShaderMap.insert_or_assign(modifyChosenPass, shaderList);
                    ImGui::PopID();
                    break;
                }
            }

            if (deleteSubstitution)
            {
                auto itFind = FRenderPassToShaderMap.find(substitution.first);
                AssertRelease(itFind != FRenderPassToShaderMap.end());
                FRenderPassToShaderMap.erase(itFind);
                ImGui::PopID();
                break;                
            }

            // Vertex Shader
            std::string vertexShader = substitution.second[0];
            EDITOR_PROPERTY_STRING(ShaderType::GetName(ShaderType::VERTEX_SHADER), vertexShader, true, "*.sc");
            if (ImGui::Button("Clear vertex shader"))
                vertexShader.clear();

            // Fragment Shader
            std::string fragmentShader = substitution.second[1];
            EDITOR_PROPERTY_STRING(ShaderType::GetName(ShaderType::FRAGMENT_SHADER), fragmentShader, true, "*.sc");
            if (ImGui::Button("Clear fragment shader"))
                fragmentShader.clear();

            substitution.second[0] = vertexShader;
            substitution.second[1] = fragmentShader;

            idx++;
            ImGui::PopID();
            ImGui::Unindent();
        }
    }
}

#ifdef PERFORM_SECURITY_CHECKS
bool ProgramDescriptorV2::UsesUniformOfType(const std::string& parName, bgfx::UniformType::Enum parType) const
{
    foreachitemconst(uniform, FUniformsAndTypes)
    {
        if (parName == uniform.first)
        {
            return parType == uniform.second;
        }
    }
    return false;
}
#endif
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
//          MaterialDescriptor
//----------------------------------------------------------------
IMPLEMENT_POOL_ALLOCATED(MaterialDescriptor);
MaterialDescriptor::MaterialDescriptor()
    : RefCountedObject()
{
}

MaterialDescriptor::~MaterialDescriptor()
{
}

//----------------------------------------------------------------
//          MaterialDescriptorV2
//----------------------------------------------------------------
IMPLEMENT_POOL_ALLOCATED(MaterialDescriptorV2);
MaterialDescriptorV2::MaterialDescriptorV2()
    : RefCountedObject()
{
}

MaterialDescriptorV2::~MaterialDescriptorV2()
{
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
