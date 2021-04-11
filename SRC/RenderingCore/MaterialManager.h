#pragma once

#include "Common/RenderingHandles.h"
#include "Common/Singleton.h"

namespace ECSEngine
{
namespace Rendering
{
class MaterialInstance;
namespace MaterialManager
{
void Initialise();
void Shutdown();
const MaterialInstanceHandle CreateMaterialInstanceIFN(const std::string& parMaterialFilename);
const MaterialInstance* GetMaterialInstance(const MaterialInstanceHandle& parHandle);
void SetSamplerUniform_IKNOWWHATIMDOING(const std::string& parUniformName, const u16& parTextureHandle, const u32 parSlot);
void SetSamplerUniform(const std::string& parUniformName, const TextureHandle& parTextureHandle, const u32 parSlot);
void SetVec4Uniform(const std::string& parUniformName, const glm::vec4& parUniformValue);
void SetMat3Uniform(const std::string& parUniformName, const glm::mat3& parUniformValue);
void SetMat4Uniform(const std::string& parUniformName, const glm::mat4& parUniformValue);
void SetMat4Uniforms(const std::string& parUniformName, const glm::mat4* parUniformValue, const u8 parNumber);
} // namespace MaterialManager

} // namespace Rendering
} // namespace ECSEngine
