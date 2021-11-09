#pragma once
#include "ShaderType.h"
#include "bgfx/bgfx.h"

namespace bgfx
{
struct ProgramHandle;
struct ShaderHandle;
} // namespace bgfx

namespace ECSEngine
{
namespace Rendering
{
#ifndef ABSOLUTELY_NOT_ASSERT
bool CompileShaders(const std::string& parFilename, const ShaderType::Type parShaderType);
#endif
bgfx::ShaderHandle loadShader(const std::string& parFilename OnlyWithAssertions(COMMA const ShaderType::Type parShaderType));
bgfx::ProgramHandle LoadProgram(const std::string& parBasePath, const std::string& parBaseProgramName);
bgfx::ProgramHandle LoadProgram(const std::string& parBasePath, const std::string& parFolderName, const std::string& parBaseProgramName);
} // namespace Rendering
} // namespace ECSEngine
