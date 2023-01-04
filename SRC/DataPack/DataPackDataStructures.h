#pragma once

namespace ECSEngine
{
namespace DataPack
{
namespace Version
{
constexpr u32 MajorVersion = 0;
constexpr u32 MinorVersion = 1;
} // namespace Version

#pragma pack(push, r1, 1)
struct DataPackHeader
{
    u32 MajorVersion = Version::MajorVersion;
    u32 MinorVersion = Version::MinorVersion;
    u32 NbFiles = 0;
};
using FileOffset = u32;

struct FileRecordHeader
{
    u32 FileByteSize = 0;
    u32 FileNameByteSize = 0;
};
#pragma pack(pop, r1)

const std::set<std::string>& GetExtensionsToPack();
const std::set<std::string>& GetSoundsExtensionsToPack();

} // namespace DataPack
} // namespace ECSEngine