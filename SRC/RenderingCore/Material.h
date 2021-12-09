#pragma once
#include "Common/MeshStreamingData.h"
#include "Common/RefCountedObject.h"
#include "Common/RenderingHandles.h"
#include "MaterialInput.h"
#include "RenderPass.h"
#include "ShaderType.h"

namespace ECSEngine
{
namespace Rendering
{
class ProgramDescriptor;
class MaterialDescriptor;
//----------------------------------------------------------------
//          Program
//----------------------------------------------------------------

class Program : public RefCountedObject
{
public:
    Program(const ProgramDescriptor* const parDescriptor);
    ~Program();

    const ProgramDescriptor* Descriptor() const { return FDescriptor; }
    const bgfx::ProgramHandle& ProgramHandle() const { return FHandle; }

    bool IsValid() const;

private:
    const ProgramDescriptor* const FDescriptor;
    bgfx::ProgramHandle FHandle;
};


//----------------------------------------------------------------
//          MaterialInstance
//----------------------------------------------------------------
class MaterialInstance final : public RefCountedObject
{
    DECLARE_POOL_ALLOCATED(MaterialInstance);

public:
    MaterialInstance(const Program* const parProgram, const MaterialDescriptor* const parMaterialDescriptor);
    ~MaterialInstance();

    const Program* GetProgram() const { return FProgram; }
    const MaterialDescriptor* GetMaterialDescriptor() const { return FMaterialDescriptor; }

    void SetTextures() const;

    void SetSamplerUniform(const std::string& parUniformName, const TextureHandle& parHandle, const u32 parSlot);
    void SetFreeFormSamplerUniform(const std::string& parUniformName, const u32& parHandle, const u32 parSlot) const;
    void SetVec4Uniform(const std::string& parUniformName, const glm::vec4& parUniformValue);
    void SetMat3Uniform(const std::string& parUniformName, const glm::mat3& parUniformValue);
    void SetMat4Uniform(const std::string& parUniformName, const glm::mat4& parUniformValue);

private:
    const Program* const FProgram;
    const MaterialDescriptor* const FMaterialDescriptor;

    std::vector<MaterialTextureInput> FTextureInput;
};
} // namespace Rendering
} // namespace ECSEngine
