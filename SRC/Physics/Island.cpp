#include "stdafx.h"

#include "Island.h"

namespace ECSEngine
{
namespace Physics
{

void Island::Init(u32 BodyCount, u32 ContactCount)
{
    Bodies.reserve(BodyCount);
}

void Island::Reset()
{
    Bodies.clear();
}

} // namespace Physics
} // namespace ECSEngine