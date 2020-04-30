#pragma once

namespace ECSEngine
{
namespace Rendering
{
class TextureDescriptor;
}

void CookMeshes(const std::vector<std::string>& parMeshFiles);
void CookMesh(const std::string& parMeshFile);

void CookTextures(const std::vector<std::string>& parTexturesDescriptorFiles);
void CookTextureBank(const std::string& parTextureBankFile);
void CookTexture(const Rendering::TextureDescriptor& parTextureDescriptor);
} // namespace ECSEngine