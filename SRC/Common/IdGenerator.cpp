#include "stdafx.h"

#include "IdGenerator.h"

namespace ECSEngine
{

IdGenerator::IdGenerator()
    : FNextIncrementalId(0)
{
}

IdGenerator::IdGenerator(IdGenerator&& other)
{
    FNextIncrementalId = other.FNextIncrementalId;
    FReusableIds = std::move(other.FReusableIds);
    other.FNextIncrementalId = -1;
}

u32 IdGenerator::GetNextId()
{
    if (FReusableIds.empty())
        return FNextIncrementalId++;
    const u32 result = FReusableIds.front();
    FReusableIds.pop();
    return result;
}

void IdGenerator::ReleaseId(const u32 parIdToRelease)
{
    FReusableIds.push(parIdToRelease);
}

void IdGenerator::operator=(IdGenerator&& other) noexcept
{
    FNextIncrementalId = other.FNextIncrementalId;
    FReusableIds = std::move(other.FReusableIds);

    other.FNextIncrementalId = -1;
}

IdGenerator::~IdGenerator()
{
}

} // namespace ECSEngine
