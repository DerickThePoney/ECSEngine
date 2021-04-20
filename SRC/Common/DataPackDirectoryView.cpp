#include "stdafx.h"

#include "DataPackDirectoryView.h"

#include "Resource.h"
#include "StringUtilities.h"

namespace ECSEngine
{

DataPackDirectoryView::DataPackDirectoryView(const std::string& path)
    : FPath(path)
{
}

DataPackDirectoryView::~DataPackDirectoryView()
{
}

bool DataPackDirectoryView::Open()
{
    return FDataPackFile.ReadDataPack(FPath);
}

u32 DataPackDirectoryView::GetRawResourceSize(const Resource& r)
{
    const u32 size = FDataPackFile.FileExists_ReturnFileSize(r.FName);
    return size;
}

u32 DataPackDirectoryView::GetRawResource(const Resource& r, c8* buffer)
{
    return FDataPackFile.CopyFileBuffer_AssumesSufficientCapacity(r.FName, buffer);
}

u32 DataPackDirectoryView::GetNumResources() const
{
    return FDataPackFile.NumberOfResources();
}

std::string DataPackDirectoryView::GetResourceName(i32 num) const
{
    AssertNotReached();
    return "";
}

void DataPackDirectoryView::ListResourceFiles(const std::string& parWildcardPattern, std::vector<std::string>& parOutFileList) const
{
    const std::map<std::string, u32>& files = FDataPackFile.FilenamesToFileOffsetMap();

    foreachitemconst(file, files)
    {
        if (StringUtilities::WildcardMatch(parWildcardPattern.c_str(), file.first.c_str()))
        {
            parOutFileList.push_back(file.first);
        }
    }
}

bool DataPackDirectoryView::FileExists(const std::string& parFileName) const
{
    const u32 size = FDataPackFile.FileExists_ReturnFileSize(parFileName);
    return size > 0 && size != -1;
}

} // namespace ECSEngine
