#include "stdafx.h"

#include "Common/ResourceCache.h"
#include "Common/ResourceFileDirectoryView.h"
#include "DataPack/DataPackReader.h"
#include "DataPack/DataPackWriter.h"

int main(int argc, char** argv)
{
    // Global resources
    ECSEngine::GlobalResourceCache::CreateIFP();
    ECSEngine::GlobalResourceCache::Instance().FCache = new ECSEngine::ResourceCache(10, new ECSEngine::ResourceFileDirectoryView("..\\Assets"));

    if (!ECSEngine::GlobalResourceCache::Instance().FCache->Initialize())
    {
        AssertNotReachedMsg("Unable to init the resource cache!!");
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

        packer.Finalize("..\\Assets.datapack");
    }

    {
        ECSEngine::DataPack::DataPackFile<ECSEngine::DataPack::Access::READ> packer;
        packer.ReadDataPack("..\\Assets.datapack");
    }

    // destroy Resources
    ECSEngine::GlobalResourceCache::Destroy();
    return 0;
}