#include "stdafx.h"

#include "BGFXRenderingUtils.h"

#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"

#include <Windows.h>
#include <codecvt>
#include <locale>

#ifndef ABSOLUTELY_NOT_ASSERT
#include "Common/ResourceFile.h"
#endif

namespace ECSEngine
{
namespace Rendering
{
#ifndef ABSOLUTELY_NOT_ASSERT
bool CompileShader(const std::string& parFilename, const ShaderType::Type parShaderType)
{
    std::cout << "Cooking " << parFilename << std::endl;

    auto pos = parFilename.find_last_of('.');
    std::string baseFile = "";
    if (pos != parFilename.npos)
    {
        baseFile = parFilename.substr(0, pos + 1) + "sc";
    }

    // additional information
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    // set the size of the structures
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    pos = parFilename.find_last_of('\\');
    std::string varyingDef = "";
    if (pos != parFilename.npos)
    {
        varyingDef = parFilename.substr(0, pos + 1) + "varying.def.sc";
    }

    std::wstring wideString = L"..\\External\\BGFX\\ToolBinaries\\shadercRelease.exe --varyingdef ";
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring varying = converter.from_bytes(ECSEngine::GlobalResourceCache::Instance().FCache->GetFileSystem()->GetBasePathName() + "\\" + varyingDef);
    std::wstring baseFilename = converter.from_bytes(ECSEngine::GlobalResourceCache::Instance().FCache->GetFileSystem()->GetBasePathName() + "\\" + baseFile);
    std::wstring compiledShader = converter.from_bytes(ECSEngine::GlobalResourceCache::Instance().FCache->GetFileSystem()->GetBasePathName() + "\\" + parFilename);

    wideString += varying + L" -f " + baseFilename + L" -o " + compiledShader + L" -p " + ((parShaderType == ShaderType::VERTEX_SHADER) ? L"vs_5_0" : L"ps_5_0") +
          L" -i ../External/BGFX/bgfx/src/ --type " + ((parShaderType == ShaderType::VERTEX_SHADER) ? L"vertex" : L"fragment") + L" --platform windows -O 3";

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
        return false;
    }
    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // TODO GetExitCodeProcess().
    bool success = true;
    DWORD exitCode;
    if (GetExitCodeProcess(pi.hProcess, &exitCode))
    {
        std::cout << exitCode << std::endl;

        success = (exitCode == 0);
    }

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return success;
}
#endif

bgfx::ShaderHandle loadShader(const std::string& parFilename OnlyWithAssertions(COMMA const ShaderType::Type parShaderType))
{
    ECSEngine::ResourceCache* cache = ECSEngine::GlobalResourceCache::Instance().FCache;
    ECSEngine::Resource shaderResource(parFilename);

#ifndef ABSOLUTELY_NOT_ASSERT
    if (!GlobalResourceCache::Instance().FCache->FileExists(&shaderResource))
    {
        if (!CompileShader(parFilename, parShaderType))
            AssertNotReached();

        GlobalResourceCache::Instance().FCache->ReOpenFileSystem();
        AssertRelease(GlobalResourceCache::Instance().FCache->FileExists(&shaderResource));
    }
#endif

    std::shared_ptr<ECSEngine::ResourceHandle> shaderDataHandle = cache->GetResourceHandle(&shaderResource);
    AssertRelease(shaderDataHandle != nullptr);

    const c8* shaderData = shaderDataHandle->Buffer();
    const u32 bufferSize = shaderDataHandle->Size();

    AssertRelease(bufferSize > 0);
    AssertRelease(shaderData != nullptr);

    const bgfx::Memory* mem = bgfx::alloc(bufferSize + 1);
    memcpy(mem->data, shaderData, bufferSize);
    mem->data[mem->size - 1] = '\0';

    return bgfx::createShader(mem);
}

bgfx::ProgramHandle LoadProgram(const std::pair<std::string, std::string>& parShaders)
{
    std::string vertexShader = parShaders.first;
    auto pos = vertexShader.find_last_of('.');
    vertexShader = vertexShader.substr(0, pos) + ".bin";

    std::string fragmentShader = parShaders.second;
    pos = fragmentShader.find_last_of('.');
    fragmentShader = fragmentShader.substr(0, pos) + ".bin";

    bgfx::ShaderHandle vsh = loadShader(vertexShader OnlyWithAssertions(COMMA ShaderType::VERTEX_SHADER));
    bgfx::ShaderHandle fsh = loadShader(fragmentShader OnlyWithAssertions(COMMA ShaderType::FRAGMENT_SHADER));

    return bgfx::createProgram(vsh, fsh, true);
}

bgfx::ProgramHandle LoadProgram(const std::string& parBasePath, const std::string& parFolderName, const std::string& parBaseProgramName)
{
    bgfx::ShaderHandle vsh = loadShader(parBasePath + parFolderName + "\\vs_" + parBaseProgramName + ".bin" OnlyWithAssertions(COMMA ShaderType::VERTEX_SHADER));
    bgfx::ShaderHandle fsh = loadShader(parBasePath + parFolderName + "\\fs_" + parBaseProgramName + ".bin" OnlyWithAssertions(COMMA ShaderType::FRAGMENT_SHADER));

    return bgfx::createProgram(vsh, fsh, true);
}

bgfx::ProgramHandle LoadProgram(const std::string& parBasePath, const std::string& parBaseProgramName)
{
    return LoadProgram(parBasePath, parBaseProgramName, parBaseProgramName);
}
} // namespace Rendering
} // namespace ECSEngine
