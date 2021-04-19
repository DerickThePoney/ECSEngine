#include "stdafx.h"

#include "Application/BaseApplication.h"
#include "Common/MainOptions.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceFileDirectoryView.h"
#include "DataPack/DataPackReader.h"
#include "DataPack/DataPackWriter.h"

int main(int argc, char** argv)
{
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

        packer.Finalize("..\\Assets.datapack");
    }

    {
        ECSEngine::DataPack::DataPackFile<ECSEngine::DataPack::Access::READ> packer;
        packer.ReadDataPack("..\\Assets.datapack");

        ECSEngine::Resource r("Configuration\\BaseApplication.json");
        u32 size = packer.FileExists_ReturnFileSize(r.FName);
        c8* buffer = new c8[size];
        packer.CopyFileBuffer_AssumesSufficientCapacity(r.FName, buffer);
        delete[] buffer;
    }

    // destroy Resources
    ECSEngine::DestroyGlobalCache();
    return 0;
}