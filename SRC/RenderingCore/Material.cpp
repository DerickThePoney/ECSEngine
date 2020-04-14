#include "stdafx.h"

#include "Material.h"

#include "BGFXRenderingUtils.h"
#include "MaterialManager.h"
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
    FUniformsAndTypes.push_back({ "color", bgfx::UniformType::Vec4 });
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
}

MaterialInstance::~MaterialInstance()
{
}

void MaterialInstance::SetSamplerUniform(const std::string& parUniformName)
{
#ifdef PERFORM_SECURITY_CHECKS
    AlwaysCheckedAssert(FProgram->Descriptor()->UsesUniformOfType(parUniformName, bgfx::UniformType::Sampler));
#endif
    MaterialManager::SetSamplerUniform(parUniformName);
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

} // namespace Rendering
} // namespace ECSEngine