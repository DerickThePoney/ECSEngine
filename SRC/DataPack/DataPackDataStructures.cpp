#include "stdafx.h"

#include "DataPackDataStructures.h"

namespace ECSEngine
{
namespace DataPack
{
static const std::set<std::string> ExtensionsToPack = { "*.fbx.gen", "*.ktx", "*.texturebank", "*.specialbank", "*.json", "*.ttf", "*.material", "*.scene", "*.program", "*.bin",
    "*.style", "*.rml", "*.rcss" };

const std::set<std::string>& GetExtensionsToPack()
{
    return ExtensionsToPack;
}
} // namespace DataPack
} // namespace ECSEngine
