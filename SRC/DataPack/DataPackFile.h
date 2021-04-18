#pragma once

namespace ECSEngine
{
namespace DataPack
{
namespace Access
{
enum TType
{
    READ,
    WRITE
};
}

template<Access::TType _Access>
class DataPackFile;

} // namespace DataPack
} // namespace ECSEngine