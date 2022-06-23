#include "stdafx.h"

#include "IdGenerator.h"

#include "SavingSystemImplementation.h"

#include <cereal/types/queue.hpp>

namespace ECSEngine
{
IMPLEMENT_VIRTUAL_SAVELOAD_ABILITIES(IdGenerator);
template<typename Chunk, bool isWriting>
void IdGenerator::SaveLoad(Chunk& parChunk)
{
    parChunk& FNextIncrementalId;
    parChunk& FReusableIds;
}

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

template<class Archive>
void IdGenerator::serialize(Archive& ar)
{
    ar(PROPERTY(NextIncrementalId), PROPERTY(ReusableIds));
}

template void IdGenerator::serialize<cereal::JSONInputArchive>(cereal::JSONInputArchive&);
template void IdGenerator::serialize<cereal::JSONOutputArchive>(cereal::JSONOutputArchive&);

} // namespace ECSEngine
