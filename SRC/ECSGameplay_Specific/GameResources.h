#pragma once

namespace ECSEngine
{
namespace GameResource
{
enum Type
{
#define RESOURCE(NAME) NAME,
#include "GameResources.inl"
#undef RESOURCE
    LENGTH
};

const char* GetName(const Type parResource);

} // namespace GameResource

} // namespace ECSEngine
