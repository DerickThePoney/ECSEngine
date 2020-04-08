#pragma once

namespace ECSEngine
{

class IdGenerator
{
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
    void serialize(Archive& ar)
    {
        ar(PROPERTY(NextIncrementalId), PROPERTY(ReusableIds));
    }

private:
    u32 FNextIncrementalId;
    std::queue<u32> FReusableIds;
};
} // namespace ECSEngine
