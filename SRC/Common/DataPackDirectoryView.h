#pragma once
#include "DataPack/DataPackReader.h"
#include "ResourceFile.h"

namespace ECSEngine
{
class DataPackDirectoryView final : public IResourceFile
{
public:
    DataPackDirectoryView(const std::string& path);
    ~DataPackDirectoryView();

    bool Open() override;
    u32 GetRawResourceSize(const Resource& r) override;
    u32 GetRawResource(const Resource& r, c8* buffer) override;
    u32 GetNumResources() const override;
    std::string GetResourceName(i32 num) const override;
    void ListResourceFiles(const std::string& parWildcardPattern, std::vector<std::string>& parOutFileList) const override;
    bool FileExists(const std::string& parFileName) const override;
    const std::string& GetBasePathName() const override { return FPath; }
    const char* FileSystemInfo() const override { return "Data packed system view"; };

private:
    DataPack::DataPackFile<DataPack::Access::READ> FDataPackFile;
    const std::string FPath;
};

} // namespace ECSEngine