#pragma once
#include "Common/PoolAllocator.h"
#include "Common/RefCountedObject.h"
#include "Common/MeshStreamingData.h"
#include "ShaderType.h"
#include "RenderPass.h"
#include "MaterialInput.h"

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

    const MeshLayoutDescription& LayoutDescription() const { return FLayoutDescription; }

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
//          MultiPassProgramDescriptor
//----------------------------------------------------------------

class MultiPassProgramDescriptor final : public RefCountedObject
{
    DECLARE_POOL_ALLOCATED(MultiPassProgramDescriptor);

public:
    using ShaderSubstitution = std::string;
    using ShaderSubstitutionList = std::array<ShaderSubstitution, ShaderType::LENGTH>;
    using RenderPassToShaderMap = std::unordered_map<RenderPassId::Type, ShaderSubstitutionList>;

public:
    MultiPassProgramDescriptor();
    ~MultiPassProgramDescriptor();

    const std::string& Filename() const { return FFilename; }
    void OverrideFilename(const std::string& parName) { FFilename = parName; }

    // IDEA: Give the exact Shaders to be used
    const RenderPassId::Type DefaultSubstitution() const { return FDefaultView; }
    const RenderPassToShaderMap& RenderPassToShaders() const { return FRenderPassToShaderMap; }
    const std::vector<std::pair<std::string, bgfx::UniformType::Enum>>& GetUniformsAndTypes() const { return FUniformsAndTypes; }

    MeshLayoutDescription LayoutDescription() const { return FLayoutDescription; }

    void DrawEditor();

#ifdef ENABLE_SECURITY_CHECKS
    bool UsesUniformOfType(const std::string& parName, bgfx::UniformType::Enum parType) const;
#endif

    SERIALIZE()
    {
        PROPERTYFIELD(Filename, "UNKNOWN");
        PROPERTYFIELD(RenderPassToShaderMap, RenderPassToShaderMap());
        PROPERTYFIELD(DefaultView, RenderPassId::GEOMETRY_PASS);
        PROPERTYFIELD(UniformsAndTypes, {});

        PROPERTYFIELD(LayoutDescription, {});

        PROPERTYFIELD(IsInstanced, false);
    }

private:
    std::string FFilename = "UNKNOWN";
    RenderPassToShaderMap FRenderPassToShaderMap;
    RenderPassId::Type FDefaultView;

    std::vector<std::pair<std::string, bgfx::UniformType::Enum>> FUniformsAndTypes; //+ More type semantic ? genre bin 0 == xxx?

    // Inputs description
    MeshLayoutDescription FLayoutDescription;

    bool FIsInstanced = false;
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
//          MutiPassMaterialDescriptor
//----------------------------------------------------------------
class MultiPassMaterialDescriptor final : public RefCountedObject
{
    DECLARE_POOL_ALLOCATED(MultiPassMaterialDescriptor);

public:
    MultiPassMaterialDescriptor();
    ~MultiPassMaterialDescriptor();

    void DrawEditor();

    const std::string& Filename() const { return FFilename; }
    const std::string& MultipassProgramDescriptorFilename() const { return FMultipassProgramDescriptorFilename; }
    const std::vector<MaterialTextureInputDescriptor>& GetTexturesInput() const { return FTexturesInputDescriptors; }

    SERIALIZE()
    {
        PROPERTYFIELD(Filename, "UNKNOWN");
        ar(PROPERTY(MultipassProgramDescriptorFilename), PROPERTY(TexturesInputDescriptors));
    }

private:
    std::string FFilename = "UNKNOWN";
    std::string FMultipassProgramDescriptorFilename;

    // Input type description?
    std::vector<MaterialTextureInputDescriptor> FTexturesInputDescriptors;
};


}
} // namespace ECSEngine