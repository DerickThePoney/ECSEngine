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
//----------------------------------------------------------------
//          ProgramDescriptor
//----------------------------------------------------------------

class ProgramDescriptor final : public RefCountedObject
{
    DECLARE_POOL_ALLOCATED(ProgramDescriptor);

public:
    ProgramDescriptor();
    ~ProgramDescriptor();

    const std::string& GetShadersBasePath() const { return FShadersBasePath; }
    const std::string& GetShadersBaseName() const { return FShadersBaseName; }
    const std::vector<std::pair<std::string, bgfx::UniformType::Enum>>& GetUniformsAndTypes() const { return FUniformsAndTypes; }

    MeshLayoutDescription LayoutDescription() const { return FLayoutDescription; }

#ifdef PERFORM_SECURITY_CHECKS
    bool UsesUniformOfType(const std::string& parName, bgfx::UniformType::Enum parType) const;
#endif

    SERIALIZE()
    {
        PROPERTYFIELD(ShadersBasePath, "");
        PROPERTYFIELD(ShadersBaseName, "");
        PROPERTYFIELD(UniformsAndTypes, {});

        PROPERTYFIELD(LayoutDescription, {});

        PROPERTYFIELD(IsInstanced, false);
    }

private:
    std::string FShadersBasePath;
    std::string FShadersBaseName;
    std::vector<std::pair<std::string, bgfx::UniformType::Enum>> FUniformsAndTypes; //+ More type semantic ? genre bin 0 == xxx?

    // Inputs description
    MeshLayoutDescription FLayoutDescription;

    bool FIsInstanced = false;
};

//----------------------------------------------------------------
//          ProgramDescriptorV2
//----------------------------------------------------------------

class ProgramDescriptorV2 final : public RefCountedObject
{
    DECLARE_POOL_ALLOCATED(ProgramDescriptorV2);

public:
    using ShaderSubstitution = std::pair<ShaderType::Type, std::string>;
    using ShaderSubstitutionList = std::vector<ShaderSubstitution>;
    using RenderPassToShaderMap = std::unordered_map<RenderPassId::Type, ShaderSubstitutionList>;

public:
    ProgramDescriptorV2();
    ~ProgramDescriptorV2();

    // IDEA: Give the exact Shaders to be used
    const RenderPassToShaderMap& RenderPassToShaders() const { return FRenderPassToShaderMap; }
    const std::vector<std::pair<std::string, bgfx::UniformType::Enum>>& GetUniformsAndTypes() const { return FUniformsAndTypes; }

    MeshLayoutDescription LayoutDescription() const { return FLayoutDescription; }

    void DrawEditor();

#ifdef ENABLE_SECURITY_CHECKS
    bool UsesUniformOfType(const std::string& parName, bgfx::UniformType::Enum parType) const;
#endif

    SERIALIZE()
    {
        PROPERTYFIELD(RenderPassToShaderMap, RenderPassToShaderMap());
        PROPERTYFIELD(DefaultView, RenderPassId::GEOMETRY_PASS);
        PROPERTYFIELD(UniformsAndTypes, {});

        PROPERTYFIELD(LayoutDescription, {});

        PROPERTYFIELD(IsInstanced, false);
    }

private:
    RenderPassToShaderMap FRenderPassToShaderMap;
    RenderPassId::Type FDefaultView;

    std::vector<std::pair<std::string, bgfx::UniformType::Enum>> FUniformsAndTypes; //+ More type semantic ? genre bin 0 == xxx?

    // Inputs description
    MeshLayoutDescription FLayoutDescription;

    bool FIsInstanced = false;
};

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
//          MaterialDescriptor
//----------------------------------------------------------------
class MaterialDescriptor final : public RefCountedObject
{
    DECLARE_POOL_ALLOCATED(MaterialDescriptor);

public:
    MaterialDescriptor();
    ~MaterialDescriptor();

    const std::string& GetProgramDescriptorName() const { return FProgramDescriptorFilename; }
    const std::vector<MaterialTextureInputDescriptor>& GetTexturesInput() const { return FTexturesInputDescriptors; }

    SERIALIZE() { ar(PROPERTY(ProgramDescriptorFilename), PROPERTY(TexturesInputDescriptors)); }

private:
    std::string FProgramDescriptorFilename;

    // Input type description?
    std::vector<MaterialTextureInputDescriptor> FTexturesInputDescriptors;
};

//----------------------------------------------------------------
//          MaterialDescriptorV2
//----------------------------------------------------------------
class MaterialDescriptorV2 final : public RefCountedObject
{
    DECLARE_POOL_ALLOCATED(MaterialDescriptorV2);

public:
    MaterialDescriptorV2();
    ~MaterialDescriptorV2();

    // IDEA: Have the material descriptor load several programs for =/= possible render passes

    void DrawEditor();

    const std::string& GetProgramDescriptorName() const { return FProgramDescriptorFilename; }
    const std::vector<MaterialTextureInputDescriptor>& GetTexturesInput() const { return FTexturesInputDescriptors; }

    SERIALIZE() { ar(PROPERTY(ProgramDescriptorFilename), PROPERTY(TexturesInputDescriptors)); }

private:
    std::string FProgramDescriptorFilename;

    // Input type description?
    std::vector<MaterialTextureInputDescriptor> FTexturesInputDescriptors;
};

//----------------------------------------------------------------
//          MaterialInstanceDescriptor
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
