#include "stdafx.h"

#include "ResourceFileDirectoryView.h"

#include "Resource.h"
#include "StringUtilities.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <locale>

namespace ECSEngine
{
ResourceFileDirectoryView::ResourceFileDirectoryView(const std::string& path)
    : FPath(path)
{
}

ResourceFileDirectoryView::~ResourceFileDirectoryView()
{
}

bool ResourceFileDirectoryView::Open()
{
    ListResources();
    return true;
}

u32 ResourceFileDirectoryView::GetRawResource(const Resource& r, c8* buffer)
{
    auto itFind = FPathToFilename.find(r.FName);
    AlwaysCheckedAssert(itFind != FPathToFilename.end());
    std::ifstream ifs(FPath + "\\" + r.FName, std::ios::binary);
    AssertRelease(ifs.good());
    ifs.read(buffer, itFind->second.FFilesize);
    return itFind->second.FFilesize;
}

u32 ResourceFileDirectoryView::GetNumResources() const
{
    return static_cast<u32>(FPathToFilename.size());
}

u32 ResourceFileDirectoryView::GetRawResourceSize(const Resource& r)
{
    auto itFind = FPathToFilename.find(r.FName);
    AlwaysCheckedAssert(itFind != FPathToFilename.end());

    if (itFind != FPathToFilename.end())
    {
        return itFind->second.FFilesize;
    }
    return 0;
}

std::string ResourceFileDirectoryView::GetResourceName(i32 num) const
{
    AssertNotReached();
    return "";
}

void ResourceFileDirectoryView::ListResources()
{
    std::filesystem::recursive_directory_iterator itDir(FPath);
    std::filesystem::recursive_directory_iterator itEnd;

    const std::filesystem::path basePath(FPath);

    for (; itDir != itEnd; ++itDir)
    {
        if (itDir->is_regular_file())
        {
            const std::filesystem::path absolute = itDir->path();
            const std::filesystem::path path = std::filesystem::relative(absolute, FPath);
            std::string filepath = path.string();
            std::transform(filepath.begin(), filepath.end(), filepath.begin(), [](unsigned char c) { return std::tolower(c); });
            AlwaysCheckedAssert(FPathToFilename.find(filepath) == FPathToFilename.end());
            FilesystemRecord record{ path.filename().string(), static_cast<u32>(std::filesystem::file_size(absolute)) };
            FPathToFilename[filepath] = record;
        }
    }
}

void ResourceFileDirectoryView::ListResourceFiles(const std::string& parWildcardPattern, std::vector<std::string>& parOutFileList) const
{
    foreachitemconst(strFileRecord, FPathToFilename)
    {
        if (StringUtilities::WildcardMatch(parWildcardPattern.c_str(), strFileRecord.second.FFilename.c_str()))
        {
            parOutFileList.push_back(strFileRecord.first);
        }
    }
}

bool ResourceFileDirectoryView::FileExists(const std::string& parFileName) const
{
    std::string lowerCaseFilename = parFileName;
    std::transform(lowerCaseFilename.begin(), lowerCaseFilename.end(), lowerCaseFilename.begin(), [](unsigned char c) { return std::tolower(c); });
    return FPathToFilename.find(lowerCaseFilename) != FPathToFilename.end();
}

} // namespace ECSEngine
