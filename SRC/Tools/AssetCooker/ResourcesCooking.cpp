#include "stdafx.h"

#include "ResourcesCooking.h"

#include "Common/Logger.h"
#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceFile.h"
#include "Common/ResourceHandle.h"
#include "RenderingCore/Texture.h"
#include "Tools/AssimpWrapper/AssimpMeshDataLoading.h"

#include <codecvt>
#include <locale>

namespace ECSEngine
{

void CookMeshes(const std::vector<std::string>& parMeshFiles)
{
    foreachitemconst(meshFile, parMeshFiles)
    {
        LOG_COOKING("Cooking mesh " + meshFile);

        CookMesh(meshFile);

        /*std::ifstream ifstr(GlobalResourceCache::Instance().FCache->GetBasePath() + "\\" + meshFile + ".gen", std::ifstream::binary);
        AssertRelease(ifstr.good());
        Rendering::MeshFileHeader fileHeader;
        ifstr.read((c8*)&fileHeader, sizeof(fileHeader));

        Rendering::VertexLayoutHash hash(fileHeader.layout);

        std::vector<glm::vec3> vertices;
        std::vector<std::vector<u32>> colors;
        std::vector<u32> indices;
        vertices.resize(fileHeader.NbVertices);
        colors.resize(fileHeader.layout.NbColorChannels);
        forrange(i, 0, fileHeader.layout.NbColorChannels) colors[i].resize(fileHeader.NbVertices);
        indices.resize(fileHeader.NbIndices);

        forrange(i, 0, fileHeader.NbVertices)
        {
            ifstr.read((c8*)&vertices[i].x, 4);
            ifstr.read((c8*)&vertices[i].y, 4);
            ifstr.read((c8*)&vertices[i].z, 4);
            forrange(j, 0, fileHeader.layout.NbColorChannels) { ifstr.read((c8*)&colors[j][i], 4); }
        }

        ifstr.read((c8*)indices.data(), 4u * fileHeader.NbIndices);*/
    }
}

void CookMesh(const std::string& parMeshFile)
{
    Resource meshResource(parMeshFile);

    std::shared_ptr<ResourceHandle> meshResourceHandle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&meshResource);
    AssertRelease(meshResourceHandle != nullptr);
    AssimpLoading::GenerateMesh(GlobalResourceCache::Instance().FCache->GetBasePath() + "\\" + parMeshFile, meshResourceHandle->Buffer(), meshResourceHandle->Size());
}

void CookTextures(const std::vector<std::string>& parTexturesDescriptorFiles)
{
    foreachitemconst(bank, parTexturesDescriptorFiles)
    {
        LOG_COOKING("Cooking texture bank : " + bank);
        CookTextureBank(bank);
    }
}

void CookTextureBank(const std::string& parTextureBankFile)
{
    Resource bank(parTextureBankFile);
    std::shared_ptr<ResourceHandle> bankResource = GlobalResourceCache::Instance().FCache->GetResourceHandle(&bank);

    AssertRelease(bankResource != nullptr);

    std::map<std::string, Rendering::TextureDescriptor> descriptors;
    {
        ResourceBuffer buf = bankResource->GetResourceBuffer();
        std::istream isstr(&buf, std::istream::in);

        cereal::JSONInputArchive ar(isstr);
        ar(NAMEDPROPERTY("Textures", descriptors));
    }

    foreachitemconst(textureDesc, descriptors) { CookTexture(textureDesc.second); }
}

void CookTexture(const Rendering::TextureDescriptor& parTextureDescriptor)
{
    std::cout << "cooking " << parTextureDescriptor.TextureFile() << std::endl;

    // additional information
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    // set the size of the structures
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    std::wstring wideString = L"..\\External\\BGFX\\bgfx\\.build\\win64_vs2019\\bin\\texturecRelease.exe -f ";
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring filename = converter.from_bytes(GlobalResourceCache::Instance().FCache->GetFileSystem()->GetBasePathName() + "\\" + parTextureDescriptor.TextureFile());
    wideString += filename + L" -o " + filename + L".ktx -t RGBA8";

    // start the program up
    if (!CreateProcess(NULL, // the path
              (LPWSTR)wideString.c_str(), // Command line
              NULL, // Process handle not inheritable
              NULL, // Thread handle not inheritable
              FALSE, // Set handle inheritance to FALSE
              0, // No creation flags
              NULL, // Use parent's environment block
              NULL, // Use parent's starting directory
              &si, // Pointer to STARTUPINFO structure
              &pi // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
              ))
    {
        return;
    }
    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // TODO GetExitCodeProcess().

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

} // namespace ECSEngine
