#pragma once

namespace ECSEngine
{
namespace Rendering
{
namespace ShaderType
{
enum Type
{
    VERTEX_SHADER,
    FRAGMENT_SHADER,
    LENGTH
};
const char* GetName(Type parPass);
Type ChooseInList(Type parPreviouslyChosen);
} // namespace ShaderType
} // namespace Rendering
} // namespace ECSEngine