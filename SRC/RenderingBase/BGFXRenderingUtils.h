#pragma once
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
bgfx::ShaderHandle loadShader(const std::string& parFilename);
bgfx::ProgramHandle LoadProgram(const std::string& parBasePath, const std::string& parBaseProgramName);
} // namespace Rendering
} // namespace ECSEngine
