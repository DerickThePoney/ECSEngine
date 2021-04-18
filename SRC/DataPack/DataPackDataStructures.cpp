#include "stdafx.h"

#include "DataPackDataStructures.h"

namespace ECSEngine
{
namespace DataPack
{
static const std::set<std::string> ExtensionsToPack = { "*.fbx.gen", "*.ktx", "*.texturebank", "*.json", "*.ttf", "*.material", "*.scene", "*.program", "*.bin", "*.style" };

const std::set<std::string>& GetExtensionsToPack()
{
    return ExtensionsToPack;
}
} // namespace DataPack
} // namespace ECSEngine
