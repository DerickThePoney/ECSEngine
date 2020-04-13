#pragma once

#include "Common/RenderingHandles.h"
#include "Common/Singleton.h"

namespace ECSEngine
{
namespace Rendering
{
class MaterialDescriptor;
class ProgramDescriptor;
class Program;
class MaterialInstance;

class MaterialManager final : public Singleton<MaterialManager>
{
public:
    MaterialManager();
    ~MaterialManager();

    void Initialise();
    void Shutdown();

    const MaterialInstanceHandle CreateMaterialInstance(const std::string& parMaterialFilename);

    const bgfx::UniformHandle& GetUniform(const std::string& parName, bgfx::UniformType::Enum parType) const;
    const MaterialInstance* GetMaterialInstance(const MaterialInstanceHandle& parHandle) const;

    void SetSamplerUniform(const std::string& parUniformName) const;
    void SetVec4Uniform(const std::string& parUniformName, const glm::vec4& parUniformValue) const;
    void SetMat3Uniform(const std::string& parUniformName, const glm::mat3& parUniformValue) const;
    void SetMat4Uniform(const std::string& parUniformName, const glm::mat4& parUniformValue) const;

private:
    std::map<std::string, u32> FFileToMaterialDescriptor;
    std::vector<MaterialDescriptor*> FMaterialDescriptors;

    std::map<std::string, u32> FFileToProgramDescriptor;
    std::vector<ProgramDescriptor*> FProgramDescriptors;
    std::vector<Program*> FPrograms;

    std::map<std::string, std::pair<bgfx::UniformHandle, bgfx::UniformType::Enum>> FUniformMap;

    std::unordered_map<MaterialInstanceHandle, MaterialInstance*> FMaterialInstances;
};
} // namespace Rendering
} // namespace ECSEngine