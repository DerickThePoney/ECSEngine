#include "stdafx.h"

#include "MaterialManager.h"

#include "Common/Logger.h"
#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceFile.h"
#include "Common/ResourceHandle.h"
#include "Material.h"

namespace ECSEngine
{
namespace Rendering
{

MaterialManager::MaterialManager()
    : Singleton()
{
}

MaterialManager::~MaterialManager()
{
}

void MaterialManager::Initialise()
{
    LOG_RENDERING("Initialising materials");

    std::vector<std::string> materialsList;
    GlobalResourceCache::Instance().FCache->GetFileSystem()->ListResourceFiles("*.material", materialsList);
    foreachitemconst(material, materialsList)
    {
        LOG_RENDERING("Loading Material " + material + "...");
        Resource res(material);
        std::shared_ptr<ResourceHandle> handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&res);

        ResourceBuffer buff = handle->GetResourceBuffer();
        std::istream istr(&buff, std::istream::binary);
        cereal::JSONInputArchive input(istr);
        MaterialDescriptor* descriptor = new MaterialDescriptor();
        input(NAMEDPROPERTY("Material", *descriptor));

        const u32 id = (u32)FMaterialDescriptors.size();
        FMaterialDescriptors.push_back(descriptor);
        FFileToMaterialDescriptor[material] = id;

        LOG_RENDERING("Loading Material " + material + "...    SUCCESS");
    }

    foreachitemconst(material, FMaterialDescriptors)
    {
        AssertRelease(material != nullptr);
        const std::string programName = material->GetProgramDescriptorName();

        auto itFind = FFileToProgramDescriptor.find(programName);
        if (itFind != FFileToProgramDescriptor.end())
        {
            LOG_RENDERING("Program " + programName + " is already loaded");
            continue;
        }

        LOG_RENDERING("Loading Program " + programName + "...");
        Resource res(programName);
        std::shared_ptr<ResourceHandle> handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&res);

        ResourceBuffer buff = handle->GetResourceBuffer();
        std::istream istr(&buff, std::istream::binary);
        cereal::JSONInputArchive input(istr);
        ProgramDescriptor* descriptor = new ProgramDescriptor();
        input(NAMEDPROPERTY("ProgramDescriptor", *descriptor));

        const u32 id = (u32)FProgramDescriptors.size();
        FProgramDescriptors.push_back(descriptor);
        FFileToProgramDescriptor[programName] = id;

        Program* program = new Program(descriptor);
        AssertRelease(program->IsValid());

        FPrograms.push_back(program);

        auto uniforms = descriptor->GetUniformsAndTypes();
        foreachitemconst(uniform, uniforms)
        {
            auto itFind = FUniformMap.find(uniform.first);
            if (itFind != FUniformMap.end())
            {
#ifdef PERFORM_SECURITY_CHECKS
                if (itFind->second.second != uniform.second)
                {
                    std::ostringstream sstr;
                    sstr << "A uniform named " << uniform.first << " was already created with type " << itFind->second.second
                         << " and we are trying to create another one with type " << uniform.second;

                    AssertNotReachedMsg(sstr.str().c_str());
                }
#endif
                continue;
            }
            else
            {
                bgfx::UniformHandle uniHandle = bgfx::createUniform(uniform.first.c_str(), uniform.second);
                AssertRelease(bgfx::isValid(uniHandle));
                FUniformMap[uniform.first] = { uniHandle, uniform.second };
            }
        }

        LOG_RENDERING("Loading Program " + programName + "...    SUCCESS");
    }
}

void MaterialManager::Shutdown()
{
    LOG_RENDERING("Finalizing materials");

    foreachitem(materialInstance, FMaterialInstances) { delete materialInstance.second; }
    FMaterialInstances.clear();

    foreachitem(program, FPrograms) { delete program; }
    foreachitem(program, FProgramDescriptors) { delete program; }
    FPrograms.clear();
    FProgramDescriptors.clear();
    FFileToProgramDescriptor.clear();

    foreachitem(material, FMaterialDescriptors) { delete material; }
    FMaterialDescriptors.clear();
    FFileToMaterialDescriptor.clear();
}

const MaterialInstanceHandle MaterialManager::CreateMaterialInstance(const std::string& parMaterialFilename)
{
    AssertRelease(FFileToMaterialDescriptor.find(parMaterialFilename) != FFileToMaterialDescriptor.end());
    const u32 materialId = FFileToMaterialDescriptor.at(parMaterialFilename);
    AssertRelease(materialId < (u32)FMaterialDescriptors.size());
    const MaterialDescriptor* const materialDescriptor = FMaterialDescriptors[materialId];
    AssertRelease(materialDescriptor != nullptr);

    const std::string& programName = materialDescriptor->GetProgramDescriptorName();
    AssertRelease(FFileToProgramDescriptor.find(programName) != FFileToProgramDescriptor.end());
    const u32 programId = FFileToProgramDescriptor.at(programName);
    AssertRelease(programId < (u32)FPrograms.size());
    const Program* const program = FPrograms[programId];
    AssertRelease(program != nullptr && program->IsValid());

    MaterialInstance* instance = new MaterialInstance(program, materialDescriptor);
    MaterialInstanceHandle handle((u32)FMaterialInstances.size());
    FMaterialInstances[handle] = instance;

    return handle;
}

const MaterialInstance* MaterialManager::GetMaterialInstance(const MaterialInstanceHandle& parHandle) const
{
    AssertRelease(FMaterialInstances.find(parHandle) != FMaterialInstances.end());
    return FMaterialInstances.at(parHandle);
}

const bgfx::UniformHandle& MaterialManager::GetUniform(const std::string& parName, bgfx::UniformType::Enum parType) const
{
    auto itFind = FUniformMap.find(parName);
    AssertRelease(itFind != FUniformMap.end());
    AssertRelease(itFind->second.second == parType);
    AssertRelease(bgfx::isValid(itFind->second.first));
    return itFind->second.first;
}

void MaterialManager::SetSamplerUniform(const std::string& parUniformName) const
{
    AssertNotReachedMsg("Not yet implemented");
}

void MaterialManager::SetVec4Uniform(const std::string& parUniformName, const glm::vec4& parUniformValue) const
{
    const bgfx::UniformHandle& handle = MaterialManager::Instance().GetUniform(parUniformName, bgfx::UniformType::Vec4);
    AssertRelease(bgfx::isValid(handle));
    bgfx::setUniform(handle, &parUniformValue[0]);
}

void MaterialManager::SetMat3Uniform(const std::string& parUniformName, const glm::mat3& parUniformValue) const
{
    const bgfx::UniformHandle& handle = GetUniform(parUniformName, bgfx::UniformType::Mat3);
    AssertRelease(bgfx::isValid(handle));
    bgfx::setUniform(handle, &parUniformValue[0][0]);
}

void MaterialManager::SetMat4Uniform(const std::string& parUniformName, const glm::mat4& parUniformValue) const
{
    const bgfx::UniformHandle& handle = MaterialManager::Instance().GetUniform(parUniformName, bgfx::UniformType::Mat4);
    AssertRelease(bgfx::isValid(handle));
    bgfx::setUniform(handle, &parUniformValue[0][0]);
}

} // namespace Rendering
} // namespace ECSEngine