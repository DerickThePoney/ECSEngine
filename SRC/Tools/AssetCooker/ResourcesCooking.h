#pragma once

namespace ECSEngine
{
namespace Rendering
{
class TextureDescriptor;
}

void CookMeshes(const std::vector<std::string>& parMeshFiles);

void CookTextures(const std::vector<std::string>& parTexturesDescriptorFiles);

void CompileShaders(const std::vector<std::string>& parShadersFiles);
} // namespace ECSEngine
