#pragma once
#include "ResourceFile.h"

namespace ECSEngine
{
struct FilesystemRecord
{
    std::string FFilename;
    u32 FFilesize;
};

class ResourceFileDirectoryView final : public IResourceFile
{
public:
    ResourceFileDirectoryView(const std::string& path);
    ~ResourceFileDirectoryView();

    bool Open() override;
    u32 GetRawResourceSize(const Resource& r) override;
    u32 GetRawResource(const Resource& r, c8* buffer) override;
    u32 GetNumResources() const override;
    std::string GetResourceName(i32 num) const override;
    void ListResourceFiles(const std::string& parWildcardPattern, std::vector<std::string>& parOutFileList) const override;

    bool FileExists(const std::string& parFileName) const override;

    const std::string& GetBasePathName() const override { return FPath; }

private:
    void ListResources();

private:
    std::string FPath;
    std::map<std::string, FilesystemRecord> FPathToFilename;
};
} // namespace ECSEngine
