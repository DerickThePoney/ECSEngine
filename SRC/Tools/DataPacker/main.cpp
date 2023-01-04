#include "stdafx.h"

#include "Common/CallStack.h"
#include "Common/MainOptions.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceFileDirectoryView.h"
#include "DataPack/DataPackReader.h"
#include "DataPack/DataPackWriter.h"

namespace ECSEngine
{
MainOptions Options;
} // namespace ECSEngine

int main(int argc, char** argv)
{
    ECSEngine::CallStack::InitializeSymbols();
#ifndef COMPILE_FINAL
    ECSEngine::Options.NoDatapack = true;
#endif
    // Global resources
    if (!ECSEngine::InitialiseGlobalCache())
    {
        ECSEngine::DestroyGlobalCache();
        return -1;
    }

    {
        ECSEngine::DataPack::DataPackFile<ECSEngine::DataPack::Access::WRITE> packer;
        const std::set<std::string>& extensions = ECSEngine::DataPack::GetExtensionsToPack();

        foreachitemconst(extension, extensions)
        {
            std::vector<std::string> files;
            ECSEngine::GlobalResourceCache::Instance().FCache->GetFileSystem()->ListResourceFiles(extension, files);

            foreachitemconst(file, files)
            {
                ECSEngine::Resource r(file);
                packer.PushFile(&r);
            }
        }

        packer.Finalize(std::string(ECSEngine::Configuration::AssetsDatapackDirectory) + ".datapack");
    }

    // destroy Resources
    ECSEngine::DestroyGlobalCache();
    ECSEngine::CallStack::CleanupSymbols();
    return 0;
}