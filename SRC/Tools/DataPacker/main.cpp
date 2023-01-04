#include "stdafx.h"

#include "Common/CallStack.h"
#include "Common/MainOptions.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceFileDirectoryView.h"
#include "DataPack/DataPackReader.h"
#include "DataPack/DataPackWriter.h"
#include "SoundCore/SoundResourceFileSystem.h"

namespace ECSEngine
{
MainOptions Options;
} // namespace ECSEngine

void PackFiles(const char* parDirectory, const std::set<std::string>& parExtensions, ECSEngine::ResourceCache* parCache)
{
    AssertRelease(parCache != nullptr);
    ECSEngine::DataPack::DataPackFile<ECSEngine::DataPack::Access::WRITE> packer;
    packer.SetResourceCache(parCache);

    foreachitemconst(extension, parExtensions)
    {
        std::vector<std::string> files;
        parCache->GetFileSystem()->ListResourceFiles(extension, files);

        foreachitemconst(file, files)
        {
            ECSEngine::Resource r(file);
            packer.PushFile(&r);
        }
    }

    packer.Finalize(std::string(parDirectory) + ".datapack");
}

int main(int argc, char** argv)
{
    ECSEngine::CallStack::InitializeSymbols();
#ifndef COMPILE_FINAL
    ECSEngine::Options.NoDatapack = true;
#endif
    {
        // Global resources
        if (!ECSEngine::InitialiseGlobalCache())
        {
            ECSEngine::DestroyGlobalCache();
            return -1;
        }

        {
            const std::set<std::string>& extensions = ECSEngine::DataPack::GetExtensionsToPack();
            PackFiles(ECSEngine::Configuration::AssetsDatapackDirectory, extensions, ECSEngine::GlobalResourceCache::Instance().FCache);
        }

        ECSEngine::DestroyGlobalCache();
    }

    {
        if (!ECSEngine::SoundResources::InitializeCache())
        {
            ECSEngine::SoundResources::DestroyCache();
            return -1;
        }

        {
            const std::set<std::string>& extensions = ECSEngine::DataPack::GetSoundsExtensionsToPack();
            PackFiles(ECSEngine::Configuration::SoundDatapackDirectory, extensions, ECSEngine::SoundResourceCache::Instance().FCache);
        }
        ECSEngine::SoundResources::DestroyCache();
    }

    // destroy Resources

    ECSEngine::CallStack::CleanupSymbols();
    return 0;
}