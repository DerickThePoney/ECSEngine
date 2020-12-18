#include "stdafx.h"

#include "ResourcesCooking.h"

#include "Common/Logger.h"
#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceFile.h"
#include "Common/ResourceHandle.h"
#include "RenderingCore/TextureBank.h"
#include "RenderingCore/TextureDescriptor.h"
#include "Tools/AssimpWrapper/AssimpMeshDataLoading.h"

#include <codecvt>
#include <locale>

namespace ECSEngine
{

namespace MeshCooking
{
void CookMesh(const std::string& parMeshFile)
{
    std::cout << "cooking " << parMeshFile << std::endl;

    Resource meshResource(parMeshFile);

    std::shared_ptr<ResourceHandle> meshResourceHandle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&meshResource);
    AssertRelease(meshResourceHandle != nullptr);
    AssimpLoading::GenerateMesh(GlobalResourceCache::Instance().FCache->GetBasePath() + "\\" + parMeshFile, meshResourceHandle->Buffer(), meshResourceHandle->Size());
}
} // namespace MeshCooking

void CookMeshes(const std::vector<std::string>& parMeshFiles)
{
    foreachitemconst(meshFile, parMeshFiles)
    {
        LOG_COOKING("Cooking mesh " + meshFile);

        MeshCooking::CookMesh(meshFile);
    }
}

namespace TextureCooking
{
void CookTexture(const std::string& parCookedTextureName, const Rendering::TextureDescriptor& parTextureDescriptor)
{
    std::cout << "cooking " << parTextureDescriptor.TextureFile() << std::endl;

    // additional information
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    // set the size of the structures
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    std::wstring wideString = L"..\\External\\BGFX\\ToolBinaries\\texturecRelease.exe -f ";
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    const std::string& file = parTextureDescriptor.TextureFile();
    std::wstring filename = converter.from_bytes(GlobalResourceCache::Instance().FCache->GetFileSystem()->GetBasePathName() + "\\" + file);

    auto pos = file.find_last_of('\\');
    std::string path = "";
    if (pos != file.npos)
    {
        path = file.substr(0, pos + 1);
    }

    std::wstring cookedFilename = converter.from_bytes(GlobalResourceCache::Instance().FCache->GetFileSystem()->GetBasePathName() + "\\" + path + parCookedTextureName);
    wideString += filename + L" -o " + cookedFilename + L".ktx";

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
        printf("CreateProcess failed (%d).\n", GetLastError());
        return;
    }
    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // TODO GetExitCodeProcess().

    DWORD exitCode;
    if (GetExitCodeProcess(pi.hProcess, &exitCode))
    {
        std::cout << exitCode << std::endl;
    }

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void CookTextureBank(const std::string& parTextureBankFile)
{
    Resource bank(parTextureBankFile);
    std::shared_ptr<ResourceHandle> bankResource = GlobalResourceCache::Instance().FCache->GetResourceHandle(&bank);

    AssertRelease(bankResource != nullptr);

    Rendering::TextureBank textureBank = Rendering::TextureBank(0);
    {
        ResourceBuffer buf = bankResource->GetResourceBuffer();
        std::istream isstr(&buf, std::istream::in);

        cereal::JSONInputArchive ar(isstr);
        ar(NAMEDPROPERTY("TextureBank", textureBank));
    }

    foreachitemconst(textureDesc, textureBank.Descriptors()) { CookTexture(textureDesc.first, textureDesc.second); }
}
} // namespace TextureCooking

void CookTextures(const std::vector<std::string>& parTexturesDescriptorFiles)
{
    foreachitemconst(bank, parTexturesDescriptorFiles)
    {
        LOG_COOKING("Cooking texture bank : " + bank);
        TextureCooking::CookTextureBank(bank);
    }
}

namespace ShaderCompiling
{
void CompileShader(const std::string& parFileName, int type)
{
    std::cout << "Compiling " << parFileName << std::endl;

    // additional information
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    // set the size of the structures
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // --varyingdef varying.def.sc -f vs_CircularBuilding.sc -o vs_CircularBuilding.bin -p vs_5_0 -i ../../../../External/BGFX/bgfx/src/ --type vertex --platform windows -O 3 -V
    std::wstring wideString = L"..\\External\\BGFX\\ToolBinaries\\shadercRelease.exe ";
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring filename = converter.from_bytes(GlobalResourceCache::Instance().FCache->GetFileSystem()->GetBasePathName() + "\\" + parFileName);

    auto pos = parFileName.find_last_of('.');
    std::string fileNoExtension = "";
    if (pos != parFileName.npos)
    {
        fileNoExtension = parFileName.substr(0, pos);
    }

    pos = parFileName.find_last_of('\\');
    std::string fileNoPath = "";
    std::string path = "";
    if (pos != parFileName.npos)
    {
        path = parFileName.substr(0, pos + 1);
        fileNoPath = parFileName.substr(pos + 1);
    }

    std::wstring cookedFilename = converter.from_bytes(GlobalResourceCache::Instance().FCache->GetFileSystem()->GetBasePathName() + "\\" + fileNoExtension + ".bin");
    std::wstring pathW = converter.from_bytes(GlobalResourceCache::Instance().FCache->GetFileSystem()->GetBasePathName() + "\\" + path);
    wideString += L"--varyingdef " + pathW + L"varying.def.sc -f " + filename + L" -o " + cookedFilename + L" -i ..\\External\\BGFX\\bgfx\\src\\ --platform windows -O 3 -V ";

    if (type == 0)
        wideString += L"-p vs_5_0 --type vertex";
    else
        wideString += L"-p ps_5_0 --type fragment";

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
        printf("CreateProcess failed (%d).\n", GetLastError());
        return;
    }
    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // TODO GetExitCodeProcess().

    DWORD exitCode;
    if (GetExitCodeProcess(pi.hProcess, &exitCode))
    {
        std::cout << exitCode << std::endl;
    }

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
} // namespace ShaderCompiling

void CompileShaders(const std::vector<std::string>& parShadersFiles)
{
    foreachitemconst(file, parShadersFiles)
    {
        auto pos = file.find_last_of('\\');
        std::string fileNoPath = "";
        if (pos != file.npos)
        {
            fileNoPath = file.substr(pos + 1);
        }

        if (fileNoPath == "varying.def.sc")
            continue;

        int type = -1;
        if (fileNoPath[0] == 'v')
            type = 0;
        if (fileNoPath[0] == 'f')
            type = 1;

        if (type == -1)
            continue;

        ShaderCompiling::CompileShader(file, type);
    }
}

} // namespace ECSEngine
