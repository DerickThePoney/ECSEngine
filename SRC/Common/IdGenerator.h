#pragma once
#include "Common/SavingSystemDeclaration.h"

namespace ECSEngine
{

class IdGenerator
{
    DECLARE_VIRTUAL_SAVELOAD_ABILITIES();

public:
    IdGenerator();
    ~IdGenerator();
    IdGenerator(IdGenerator&& other);

    void operator=(IdGenerator&& other) noexcept;

    IdGenerator(const IdGenerator& other) = delete;
    void operator=(const IdGenerator& other) = delete;

    u32 GetNextId();
    void ReleaseId(const u32 parIdToRelease);

    template<class Archive>
    void serialize(Archive& ar);

private:
    u32 FNextIncrementalId;
    std::queue<u32> FReusableIds;
};

} // namespace ECSEngine
