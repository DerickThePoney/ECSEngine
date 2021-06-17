#include "stdafx.h"

#include "UIMaterialsHelpers.h"

#include "RenderingCore/MaterialManager.h"

namespace ECSEngine
{
namespace UI
{

Rendering::MaterialInstanceHandle GetVertexColorMaterial()
{
    return Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\uivertexcolormaterial.material");
}

Rendering::MaterialInstanceHandle GetTextMaterial()
{
    return Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\uisimpletextmaterial.material");
}

} // namespace UI
} // namespace ECSEngine