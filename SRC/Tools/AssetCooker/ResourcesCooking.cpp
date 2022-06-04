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
#include "teeny-sha1.c"

#include <codecvt>
#include <fstream>
#include <locale>
#include <thread>

namespace ECSEngine
{
namespace ResourceCheck
{
bool HasFileChanged(const std::string& parFile)
{
    Resource resourceSha1(parFile + ".sha1");
    c8 hexdigest[41];
    Resource resource(parFile);
    std::shared_ptr<ResourceHandle> resourceHandle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&resource);
    AssertRelease(resourceHandle != nullptr);

    i32 resSha1 = sha1digest(nullptr, &hexdigest[0], reinterpret_cast<const u8*>(resourceHandle->Buffer()), resourceHandle->Size());
    AssertRelease(resSha1 == 0);

    if (GlobalResourceCache::Instance().FCache->FileExists(&resourceSha1))
    {
        std::shared_ptr<ResourceHandle> resourceHandleSha1 = GlobalResourceCache::Instance().FCache->GetResourceHandle(&resourceSha1);
        AssertRelease(resourceHandleSha1 != nullptr);

        i32 res = memcmp(&hexdigest[0], resourceHandleSha1->Buffer(), 41);
        if (res == 0)
            return false;
    }

    std::ofstream ofstr(GlobalResourceCache::Instance().FCache->GetBasePath() + "\\" + parFile + ".sha1", std::ofstream::binary);
    AssertRelease(ofstr.good());
    ofstr.write(&hexdigest[0], 41);
    return true;
}
} // namespace ResourceCheck

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
#if 1
    std::vector<std::string> meshToRecompute;
    meshToRecompute.reserve(parMeshFiles.size());
    foreachitemconst(mesh, parMeshFiles)
    {
        if (!ResourceCheck::HasFileChanged(mesh))
            continue;
        meshToRecompute.push_back(mesh);
    }

    std::vector<std::thread> threads;
    const u32 maxCpus = std::thread::hardware_concurrency() - 1;
    threads.reserve(maxCpus);

    const u32 numberOfFilesPerThreads = glm::max((u32)meshToRecompute.size() / maxCpus, 10u);

    u32 currentFileStart = 0;
    u32 currentThread = 0;
    while (currentFileStart < meshToRecompute.size() && currentThread < maxCpus)
    {
        const u32 thisEnd = glm::min(currentFileStart + numberOfFilesPerThreads, (u32)meshToRecompute.size());
        threads.push_back(std::move(std::thread(
              [&meshToRecompute, currentFileStart, thisEnd]
              {
                  std::cout << "Cooking mesh thread start" << std::endl;
                  forrange(i, currentFileStart, thisEnd) { MeshCooking::CookMesh(meshToRecompute[i]); }
              })));
        currentFileStart += numberOfFilesPerThreads;
        currentThread++;
    }

    if (currentFileStart < meshToRecompute.size())
    {
        forrange(i, currentFileStart, meshToRecompute.size())
        {
            LOG_COOKING("Cooking mesh " + meshToRecompute[i]);
            MeshCooking::CookMesh(meshToRecompute[i]);
        }
    }

    foreachitem(th, threads)
    {
        AssertRelease(th.joinable());
        th.join();
    }
#else
    foreachitemconst(meshFile, parMeshFiles)
    {
        LOG_COOKING("Cooking mesh " + meshFile);
        if (!ResourceCheck::HasFileChanged(meshFile))
            continue;
        MeshCooking::CookMesh(meshFile);
    }
#endif
}

namespace TextureCooking
{
void CookTexture(const std::string& parCookedTextureName, const std::string& parTextureDescriptor, PROCESS_INFORMATION& pi)
{
    std::cout << "cooking " << parTextureDescriptor << std::endl;

    // additional information
    STARTUPINFO si;

    // set the size of the structures
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    std::wstring wideString = L"..\\External\\BGFX\\ToolBinaries\\texturecRelease.exe -f ";
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    const std::string& file = parTextureDescriptor;
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
    std::vector<PROCESS_INFORMATION> processes;
    processes.reserve(textureBank.Descriptors().size());
    foreachitemconst(textureDesc, textureBank.Descriptors())
    {
        if (!ResourceCheck::HasFileChanged(textureDesc.second.TextureFile()))
            continue;
        processes.push_back(PROCESS_INFORMATION());
        CookTexture(textureDesc.first, textureDesc.second.TextureFile(), processes.back());
    }

    foreachitem(pi, processes)
    { // Wait until child process exits.
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

void CookFreeFormTextures(const std::string& parFreeFormTextures)
{
    Resource freeForm(parFreeFormTextures);
    std::shared_ptr<ResourceHandle> freeFormRH = GlobalResourceCache::Instance().FCache->GetResourceHandle(&freeForm);
    AssertRelease(freeFormRH != nullptr);
    ResourceBuffer buff = freeFormRH->GetResourceBuffer();
    std::istream istr(&buff, std::istream::in);

    std::vector<PROCESS_INFORMATION> processes;
    while (!istr.eof())
    {
        std::string file, name;
        istr >> file >> name;
        if (!ResourceCheck::HasFileChanged(file))
            continue;
        processes.emplace_back();
        TextureCooking::CookTexture(name, file, processes.back());
    }

    foreachitem(pi, processes)
    { // Wait until child process exits.
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
}

namespace ShaderCompiling
{
void CompileShader(const std::string& parFileName, int type, PROCESS_INFORMATION& pi)
{
    std::cout << "Compiling " << parFileName << std::endl;

    // additional information
    STARTUPINFO si;

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
}
} // namespace ShaderCompiling

void CompileShaders(const std::vector<std::string>& parShadersFiles)
{
    std::vector<PROCESS_INFORMATION> processes;
    processes.reserve(parShadersFiles.size());
    foreachitemconst(file, parShadersFiles)
    {
        if (!ResourceCheck::HasFileChanged(file))
            continue;
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

        processes.emplace_back();
        ShaderCompiling::CompileShader(file, type, processes.back());
    }

    foreachitem(pi, processes)
    { // Wait until child process exits.
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
}

} // namespace ECSEngine
