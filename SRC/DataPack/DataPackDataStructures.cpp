#include "stdafx.h"

#include "DataPackDataStructures.h"

namespace ECSEngine
{
namespace DataPack
{
static const std::set<std::string> ExtensionsToPack = { "*.fbx.gen", "*.ktx", "*.texturebank", "*.specialbank", "*.json", "*.ttf", "*.material", "*.materialv2", "*.scene",
    "*.program", "*.programv2", "*.bin", "*.style", "*.rml", "*.rcss" };

const std::set<std::string>& GetExtensionsToPack()
{
    return ExtensionsToPack;
}

static const std::set<std::string> SoundsToPack = { "*.wav" };

const std::set<std::string>& GetSoundsExtensionsToPack()
{
    return SoundsToPack;
}
} // namespace DataPack
} // namespace ECSEngine
