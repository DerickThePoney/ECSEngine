#include "stdafx.h"

#include "BGFXRenderingUtils.h"

#include "Resource.h"
#include "ResourceCache.h"
#include "ResourceHandle.h"

namespace ECSEngine
{
namespace Rendering
{

bgfx::ShaderHandle loadShader(const std::string& parFilename)
{
    ECSEngine::ResourceCache* cache = ECSEngine::GlobalResourceCache::Instance().FCache;
    ECSEngine::Resource shaderResource(parFilename);
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

bgfx::ProgramHandle LoadProgram(const std::string& parBasePath, const std::string& parBaseProgramName)
{
    bgfx::ShaderHandle vsh = loadShader(parBasePath + parBaseProgramName + "\\vs_" + parBaseProgramName + ".bin");
    bgfx::ShaderHandle fsh = loadShader(parBasePath + parBaseProgramName + "\\fs_" + parBaseProgramName + ".bin");

    return bgfx::createProgram(vsh, fsh, true);
}

} // namespace Rendering
} // namespace ECSEngine