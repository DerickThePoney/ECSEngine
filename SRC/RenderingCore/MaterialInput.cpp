#include "stdafx.h"

#include "MaterialInput.h"

#include "MaterialManager.h"

namespace ECSEngine
{
namespace Rendering
{

MaterialTextureInputDescriptor::MaterialTextureInputDescriptor()
    : FTextureSamplerName("")
{
}

MaterialTextureInputDescriptor::~MaterialTextureInputDescriptor()
{
}

MaterialTextureInput::MaterialTextureInput(const TextureHandle& parHandle, const std::string& parSlotName, const u32 parSlot)
    : FHandle(parHandle)
    , FTextureSamplerName(parSlotName)
    , FTextureSlot(parSlot)
{
    AssertRelease(FHandle.IsValid());
}

MaterialTextureInput::~MaterialTextureInput()
{
}

void MaterialTextureInput::SetTexture() const
{
    AssertRelease(FHandle.IsValid());
    MaterialManager::SetSamplerUniform(FTextureSamplerName, FHandle, FTextureSlot);
}

} // namespace Rendering
} // namespace ECSEngine