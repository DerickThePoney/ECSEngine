#include "stdafx.h"

#include "MaterialManager.h"

#include "Common/Logger.h"
#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceFile.h"
#include "Common/ResourceHandle.h"
#include "Material.h"
#include "MaterialDescriptors.h"
#include "Texture.h"
#include "TexturesManager.h"

namespace ECSEngine
{
namespace Rendering
{

class MaterialManagerSingleton final : public Singleton<MaterialManagerSingleton>
{
public:
    MaterialManagerSingleton();
    ~MaterialManagerSingleton();

    void Initialise();
    void Shutdown();

    const MaterialInstanceHandle CreateMaterialInstanceIFN(const std::string& parMaterialFilename);
    const MultiPassMaterialInstanceHandle CreateMultiPassMaterialInstanceIFN(const std::string& parMaterialFilename);

    const bgfx::UniformHandle& GetUniform(const std::string& parName, bgfx::UniformType::Enum parType) const;
    const MaterialInstance* GetMaterialInstance(const MaterialInstanceHandle& parHandle) const;
    const MultiPassMaterialInstance* GetMultiPassMaterialInstance(const MultiPassMaterialInstanceHandle& parHandle) const;

    void SetSamplerUniform(const std::string& parUniformName, const u16& parTextureHandle, const u32 parSlot) const;
    void SetSamplerUniform(const std::string& parUniformName, const TextureHandle& parTextureHandle, const u32 parSlot) const;
    void SetFreeFormSamplerUniform(const std::string& parUniformName, const u32& parTextureHandle, const u32 parSlot) const;
    void SetVec4Uniform(const std::string& parUniformName, const vec4& parUniformValue) const;
    void SetVec4Uniforms(const std::string& parUniformName, const vec4* parUniformValue, const u16 parNumber) const;
    void SetMat3Uniform(const std::string& parUniformName, const mat3& parUniformValue) const;
    void SetMat4Uniform(const std::string& parUniformName, const mat4& parUniformValue) const;
    void SetMat4Uniforms(const std::string& parUniformName, const mat4* parUniformValue, const u8 parNumber) const;

    std::vector<MultiPassProgramDescriptor*>& GetProgramsForEditor();
    std::vector<MultiPassMaterialDescriptor*>& GetMaterialsForEditor();

private:
    std::unordered_map<std::string, u32> FFileToMaterialDescriptor;
    std::unordered_map<std::string, u32> FFileToMultiPassMaterialDescriptor;
    std::vector<MaterialDescriptor*> FMaterialDescriptors;
    std::vector<MultiPassMaterialDescriptor*> FMultiPassMaterialDescriptors;

    std::unordered_map<std::string, u32> FFileToProgramDescriptor;
    std::unordered_map<std::string, u32> FFileToMultiPassProgramDescriptor;
    std::vector<ProgramDescriptor*> FProgramDescriptors;
    std::vector<MultiPassProgramDescriptor*> FMultiPassProgramDescriptors;

    std::vector<Program*> FPrograms;
    std::vector<MultiPassProgram*> FMultiPassPrograms;

    std::unordered_map<std::string, std::pair<bgfx::UniformHandle, bgfx::UniformType::Enum>> FUniformMap;

    std::unordered_map<std::string, std::list<MaterialInstanceHandle>> FMaterialDescriptorToMaterialInstance;
    std::unordered_map<MaterialInstanceHandle, MaterialInstance*> FMaterialInstances;

    std::unordered_map<std::string, std::list<MultiPassMaterialInstanceHandle>> FMultiPassMaterialDescriptorsToMultiPassMaterialInstances;
    std::unordered_map<MultiPassMaterialInstanceHandle, MultiPassMaterialInstance*> FMultiPassMaterialInstances;
};

MaterialManagerSingleton::MaterialManagerSingleton()
    : Singleton()
{
}

MaterialManagerSingleton::~MaterialManagerSingleton()
{
}

void MaterialManagerSingleton::Initialise()
{
    LOG_RENDERING("Initialising programs v2");
    std::vector<std::string> programsList;
    GlobalResourceCache::Instance().FCache->GetFileSystem()->ListResourceFiles("*.programv2", programsList);
    foreachitemconst(programDescFile, programsList)
    {
        LOG_RENDERING(std::format("Loading multipass programs {}...", programDescFile));
        Resource res(programDescFile);
        std::shared_ptr<ResourceHandle> handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&res);

        ResourceBuffer buff = handle->GetResourceBuffer();
        std::istream istr(&buff, std::istream::binary);
        cereal::JSONInputArchive input(istr);
        MultiPassProgramDescriptor* programDesc = new MultiPassProgramDescriptor();
        input(*programDesc);
        FMultiPassProgramDescriptors.push_back(programDesc);

        MultiPassProgram* program = new MultiPassProgram(programDesc);

        FMultiPassPrograms.push_back(program);

        FFileToMultiPassProgramDescriptor[programDescFile] = FFileToMultiPassProgramDescriptor.size();

        auto uniforms = programDesc->GetUniformsAndTypes();
        foreachitemconst(uniform, uniforms)
        {
            auto itFind = FUniformMap.find(uniform.first);
            if (itFind != FUniformMap.end())
            {
#ifdef PERFORM_SECURITY_CHECKS
                if (itFind->second.second != uniform.second)
                {
                    const std::string message = std::format("A uniform named {} was already created with type {} and we are trying to create another one with type {}",
                          uniform.first, (int)itFind->second.second, (int)uniform.second);

                    AssertNotReachedMsg(message.c_str());
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

        LOG_RENDERING(std::format("Loading Multi Pass Program {}...    SUCCESS", programDescFile));
    }

    LOG_RENDERING("Initialising materials");

    std::vector<std::string> materialsList;
    GlobalResourceCache::Instance().FCache->GetFileSystem()->ListResourceFiles("*.material", materialsList);
    foreachitemconst(material, materialsList)
    {
        LOG_RENDERING(std::format("Loading Material {}...", material));
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

        LOG_RENDERING(std::format("Loading Material {}...    SUCCESS", material));
    }

    std::vector<std::string> materialsV2List;
    GlobalResourceCache::Instance().FCache->GetFileSystem()->ListResourceFiles("*.materialv2", materialsV2List);
    foreachitemconst(material, materialsV2List)
    {
        LOG_RENDERING(std::format("Loading Multi Pass Material {}...", material));
        Resource res(material);
        std::shared_ptr<ResourceHandle> handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&res);

        ResourceBuffer buff = handle->GetResourceBuffer();
        std::istream istr(&buff, std::istream::binary);
        cereal::JSONInputArchive input(istr);
        MultiPassMaterialDescriptor* descriptor = new MultiPassMaterialDescriptor();
        input(*descriptor);

        const u32 id = (u32)FMultiPassMaterialDescriptors.size();
        FMultiPassMaterialDescriptors.push_back(descriptor);
        FFileToMultiPassMaterialDescriptor[material] = id;

        LOG_RENDERING(std::format("Loading Multi Pass Material {}...    SUCCESS", material));
    }

    foreachitemconst(material, FMaterialDescriptors)
    {
        AssertRelease(material != nullptr);
        const std::string programName = material->GetProgramDescriptorName();

        auto itFind = FFileToProgramDescriptor.find(programName);
        if (itFind != FFileToProgramDescriptor.end())
        {
            LOG_RENDERING(std::format("Program {} is already loaded", programName));
            continue;
        }

        LOG_RENDERING(std::format("Loading Program {}...", programName));
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
                    const std::string message = std::format("A uniform named {} was already created with type {} and we are trying to create another one with type {}",
                          uniform.first, (int)itFind->second.second, (int)uniform.second);

                    AssertNotReachedMsg(message.c_str());
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

        LOG_RENDERING(std::format("Loading Program {}...    SUCCESS", programName));
    }
}

void MaterialManagerSingleton::Shutdown()
{
    LOG_RENDERING("Finalizing materials");

    foreachitem(uniform, FUniformMap)
    {
        bgfx::destroy(uniform.second.first);
    }

    foreachitem(materialInstance, FMaterialInstances)
    {
        delete materialInstance.second;
    }
    FMaterialInstances.clear();

    foreachitem(program, FMultiPassPrograms)
    {
        delete program;
    }
    foreachitem(program, FMultiPassProgramDescriptors)
    {
        delete program;
    }
    FMultiPassPrograms.clear();
    FMultiPassProgramDescriptors.clear();

    foreachitem(program, FPrograms)
    {
        delete program;
    }
    foreachitem(program, FProgramDescriptors)
    {
        delete program;
    }
    FPrograms.clear();
    FProgramDescriptors.clear();
    FFileToProgramDescriptor.clear();

    foreachitem(material, FMaterialDescriptors)
    {
        delete material;
    }
    foreachitem(material, FMultiPassMaterialDescriptors)
    {
        delete material;
    }
    FMaterialDescriptors.clear();
    FMultiPassMaterialDescriptors.clear();
    FFileToMaterialDescriptor.clear();
    FFileToMultiPassMaterialDescriptor.clear();
}

const MaterialInstanceHandle MaterialManagerSingleton::CreateMaterialInstanceIFN(const std::string& parMaterialFilename)
{
    // TODO CHECK IF MULTIPLE INSTANCE HANDLES ARE NEEDED IE --> SAME DESCRIPTOR, SAME PROGRAM, NO INSTANCING? OR JUST DO THE CHECK LATER ON, INSTANCES SEEM CHEAP?
    auto itFind = FMaterialDescriptorToMaterialInstance.find(parMaterialFilename);
    if (itFind != FMaterialDescriptorToMaterialInstance.end())
    {
        return *(itFind->second.begin());
    }

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

    FMaterialDescriptorToMaterialInstance[parMaterialFilename].push_back(handle);

    return handle;
}

const MultiPassMaterialInstanceHandle MaterialManagerSingleton::CreateMultiPassMaterialInstanceIFN(const std::string& parMaterialFilename)
{
    // TODO CHECK IF MULTIPLE INSTANCE HANDLES ARE NEEDED IE --> SAME DESCRIPTOR, SAME PROGRAM, NO INSTANCING? OR JUST DO THE CHECK LATER ON, INSTANCES SEEM CHEAP?
    auto itFind = FMultiPassMaterialDescriptorsToMultiPassMaterialInstances.find(parMaterialFilename);
    if (itFind != FMultiPassMaterialDescriptorsToMultiPassMaterialInstances.end())
    {
        return *(itFind->second.begin());
    }

    AssertRelease(FFileToMultiPassMaterialDescriptor.find(parMaterialFilename) != FFileToMultiPassMaterialDescriptor.end());
    const u32 materialId = FFileToMultiPassMaterialDescriptor.at(parMaterialFilename);
    AssertRelease(materialId < (u32)FMultiPassMaterialDescriptors.size());
    const MultiPassMaterialDescriptor* const materialDescriptor = FMultiPassMaterialDescriptors[materialId];
    AssertRelease(materialDescriptor != nullptr);

    const std::string& programName = materialDescriptor->MultipassProgramDescriptorFilename();
    AssertRelease(FFileToMultiPassProgramDescriptor.find(programName) != FFileToMultiPassProgramDescriptor.end());
    const u32 programId = FFileToMultiPassProgramDescriptor.at(programName);
    AssertRelease(programId < (u32)FPrograms.size());
    const MultiPassProgram* const program = FMultiPassPrograms[programId];
    AssertRelease(program != nullptr && program->IsValid());

    MultiPassMaterialInstance* instance = new MultiPassMaterialInstance(program, materialDescriptor);
    MultiPassMaterialInstanceHandle handle((u32)FMultiPassMaterialInstances.size());
    FMultiPassMaterialInstances[handle] = instance;

    FMultiPassMaterialDescriptorsToMultiPassMaterialInstances[parMaterialFilename].push_back(handle);

    return handle;
}

const MaterialInstance* MaterialManagerSingleton::GetMaterialInstance(const MaterialInstanceHandle& parHandle) const
{
    AssertRelease(FMaterialInstances.find(parHandle) != FMaterialInstances.end());
    return FMaterialInstances.at(parHandle);
}

const MultiPassMaterialInstance* MaterialManagerSingleton::GetMultiPassMaterialInstance(const MultiPassMaterialInstanceHandle& parHandle) const
{
    AssertRelease(FMultiPassMaterialInstances.find(parHandle) != FMultiPassMaterialInstances.end());
    return FMultiPassMaterialInstances.at(parHandle);
}

const bgfx::UniformHandle& MaterialManagerSingleton::GetUniform(const std::string& parName, bgfx::UniformType::Enum parType) const
{
    auto itFind = FUniformMap.find(parName);
    AssertRelease(itFind != FUniformMap.end());
    AssertRelease(itFind->second.second == parType);
    AssertRelease(bgfx::isValid(itFind->second.first));
    return itFind->second.first;
}

void MaterialManagerSingleton::SetSamplerUniform(const std::string& parUniformName, const TextureHandle& parTextureHandle, const u32 parSlot) const
{
    const bgfx::UniformHandle& handle = MaterialManagerSingleton::Instance().GetUniform(parUniformName, bgfx::UniformType::Sampler);
    AssertRelease(bgfx::isValid(handle));
    const Texture* texture = TextureManager::Instance().GetTexture(parTextureHandle);
    AssertRelease(texture != nullptr);
    bgfx::setTexture(parSlot, handle, texture->Handle());
}

void MaterialManagerSingleton::SetSamplerUniform(const std::string& parUniformName, const u16& parTextureHandle, const u32 parSlot) const
{
    const bgfx::UniformHandle& handle = MaterialManagerSingleton::Instance().GetUniform(parUniformName, bgfx::UniformType::Sampler);
    AssertRelease(bgfx::isValid(handle));
    bgfx::TextureHandle texture;
    texture.idx = parTextureHandle;
    AssertRelease(bgfx::isValid(texture));
    bgfx::setTexture(parSlot, handle, texture);
}

void MaterialManagerSingleton::SetFreeFormSamplerUniform(const std::string& parUniformName, const u32& parTextureHandle, const u32 parSlot) const
{
    const bgfx::UniformHandle& handle = MaterialManagerSingleton::Instance().GetUniform(parUniformName, bgfx::UniformType::Sampler);
    AssertRelease(bgfx::isValid(handle));

    if (parTextureHandle != 0)
    {
        const Texture* texture = TextureManager::Instance().GetFreeFormTexture(parTextureHandle - 1);
        AssertRelease(texture != nullptr);
        bgfx::setTexture(parSlot, handle, texture->Handle());
    }
    /*else
    {
        bgfx::setTexture(parSlot, handle, BGFX_INVALID_HANDLE);
    }*/
}

void MaterialManagerSingleton::SetVec4Uniform(const std::string& parUniformName, const vec4& parUniformValue) const
{
    const bgfx::UniformHandle& handle = MaterialManagerSingleton::Instance().GetUniform(parUniformName, bgfx::UniformType::Vec4);
    AssertRelease(bgfx::isValid(handle));
    bgfx::setUniform(handle, &parUniformValue);
}

void MaterialManagerSingleton::SetVec4Uniforms(const std::string& parUniformName, const vec4* parUniformValue, const u16 parNumber) const
{
    const bgfx::UniformHandle& handle = MaterialManagerSingleton::Instance().GetUniform(parUniformName, bgfx::UniformType::Vec4);
    AssertRelease(bgfx::isValid(handle));
    bgfx::setUniform(handle, parUniformValue, parNumber);
}

void MaterialManagerSingleton::SetMat3Uniform(const std::string& parUniformName, const mat3& parUniformValue) const
{
    const bgfx::UniformHandle& handle = GetUniform(parUniformName, bgfx::UniformType::Mat3);
    AssertRelease(bgfx::isValid(handle));
    bgfx::setUniform(handle, &parUniformValue.FValues[0]);
}

void MaterialManagerSingleton::SetMat4Uniform(const std::string& parUniformName, const mat4& parUniformValue) const
{
    const bgfx::UniformHandle& handle = MaterialManagerSingleton::Instance().GetUniform(parUniformName, bgfx::UniformType::Mat4);
    AssertRelease(bgfx::isValid(handle));
    bgfx::setUniform(handle, &parUniformValue.FValues[0]);
}

void MaterialManagerSingleton::SetMat4Uniforms(const std::string& parUniformName, const mat4* parUniformValue, const u8 parNumber) const
{
    const bgfx::UniformHandle& handle = MaterialManagerSingleton::Instance().GetUniform(parUniformName, bgfx::UniformType::Mat4);
    AssertRelease(bgfx::isValid(handle));
    bgfx::setUniform(handle, parUniformValue, parNumber);
}

std::vector<MultiPassProgramDescriptor*>& MaterialManagerSingleton::GetProgramsForEditor()
{
    return FMultiPassProgramDescriptors;
}

std::vector<MultiPassMaterialDescriptor*>& MaterialManagerSingleton::GetMaterialsForEditor()
{
    return FMultiPassMaterialDescriptors;
}

namespace MaterialManager
{

void Initialise()
{
    AssertRelease(!MaterialManagerSingleton::HasInstance());
    MaterialManagerSingleton::CreateIFP();
    AssertRelease(MaterialManagerSingleton::HasInstance());
    MaterialManagerSingleton::Instance().Initialise();
}

void Shutdown()
{
    AssertRelease(MaterialManagerSingleton::HasInstance());
    MaterialManagerSingleton::Instance().Shutdown();
    MaterialManagerSingleton::Destroy();
    AssertRelease(!MaterialManagerSingleton::HasInstance());
}

const MaterialInstanceHandle CreateMaterialInstanceIFN(const std::string& parMaterialFilename)
{
    AssertRelease(MaterialManagerSingleton::HasInstance());
    return MaterialManagerSingleton::Instance().CreateMaterialInstanceIFN(parMaterialFilename);
}

const MultiPassMaterialInstanceHandle CreateMultiPassMaterialInstanceIFN(const std::string& parMaterialFilename)
{
    AssertRelease(MaterialManagerSingleton::HasInstance());
    return MaterialManagerSingleton::Instance().CreateMultiPassMaterialInstanceIFN(parMaterialFilename);
}

const MaterialInstance* GetMaterialInstance(const MaterialInstanceHandle& parHandle)
{
    AssertRelease(MaterialManagerSingleton::HasInstance());
    return MaterialManagerSingleton::Instance().GetMaterialInstance(parHandle);
}

const MultiPassMaterialInstance* GetMultiPassMaterialInstance(const MultiPassMaterialInstanceHandle& parHandle)
{
    AssertRelease(MaterialManagerSingleton::HasInstance());
    return MaterialManagerSingleton::Instance().GetMultiPassMaterialInstance(parHandle);
}

void SetSamplerUniform_IKNOWWHATIMDOING(const std::string& parUniformName, const u16& parTextureHandle, const u32 parSlot)
{
    AssertRelease(MaterialManagerSingleton::HasInstance());
    MaterialManagerSingleton::Instance().SetSamplerUniform(parUniformName, parTextureHandle, parSlot);
}

void SetSamplerUniform(const std::string& parUniformName, const TextureHandle& parTextureHandle, const u32 parSlot)
{
    AssertRelease(MaterialManagerSingleton::HasInstance());
    MaterialManagerSingleton::Instance().SetSamplerUniform(parUniformName, parTextureHandle, parSlot);
}

void SetFreeFormSamplerUniform(const std::string& parUniformName, const u32 parTextureHandle, const u32 parSlot)
{
    AssertRelease(MaterialManagerSingleton::HasInstance());
    MaterialManagerSingleton::Instance().SetFreeFormSamplerUniform(parUniformName, parTextureHandle, parSlot);
}

void SetVec4Uniform(const std::string& parUniformName, const vec4& parUniformValue)
{
    AssertRelease(MaterialManagerSingleton::HasInstance());
    MaterialManagerSingleton::Instance().SetVec4Uniform(parUniformName, parUniformValue);
}

void SetVec4Uniforms(const std::string& parUniformName, const vec4* parUniformValue, const u16 parNumber)
{
    AssertRelease(MaterialManagerSingleton::HasInstance());
    MaterialManagerSingleton::Instance().SetVec4Uniforms(parUniformName, parUniformValue, parNumber);
}

void SetMat3Uniform(const std::string& parUniformName, const mat3& parUniformValue)
{
    AssertRelease(MaterialManagerSingleton::HasInstance());
    MaterialManagerSingleton::Instance().SetMat3Uniform(parUniformName, parUniformValue);
}

void SetMat4Uniform(const std::string& parUniformName, const mat4& parUniformValue)
{
    AssertRelease(MaterialManagerSingleton::HasInstance());
    MaterialManagerSingleton::Instance().SetMat4Uniform(parUniformName, parUniformValue);
}

void SetMat4Uniforms(const std::string& parUniformName, const mat4* parUniformValue, const u8 parNumber)
{
    AssertRelease(MaterialManagerSingleton::HasInstance());
    MaterialManagerSingleton::Instance().SetMat4Uniforms(parUniformName, parUniformValue, parNumber);
}

std::vector<MultiPassProgramDescriptor*>& GetProgramsForEditor()
{
    AssertRelease(MaterialManagerSingleton::HasInstance());
    return MaterialManagerSingleton::Instance().GetProgramsForEditor();
}

std::vector<MultiPassMaterialDescriptor*>& GetMaterialsForEditor()
{
    AssertRelease(MaterialManagerSingleton::HasInstance());
    return MaterialManagerSingleton::Instance().GetMaterialsForEditor();
}

} // namespace MaterialManager

} // namespace Rendering
} // namespace ECSEngine
