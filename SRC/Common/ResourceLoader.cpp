#include "stdafx.h"

#include "ResourceLoader.h"

namespace ECSEngine
{

bool DefaultResourceLoader::UseRawFile() const
{
    return true;
}

u32 ECSEngine::DefaultResourceLoader::GetLoadedResourceSize(const c8* parRawBuffer, u32 parRawSize)
{
    return parRawSize;
}

bool ECSEngine::DefaultResourceLoader::LoadResource(c8* parRawBuffer, u32 parRawSize, std::shared_ptr<ResourceHandle> parHandle)
{
    return true;
}
} // namespace ECSEngine
