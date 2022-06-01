#pragma once
#include "Common/BitSet.h"
#include "MaxModuleNumber.h"


namespace ECSEngine
{
struct EntityModuleKey
{
public:
    EntityModuleKey()
        : FKey(0)
    {
    }

    template<typename T>
    const bool HasModule() const;
    const bool HasModule(const u32 parModuleId) const;

    template<typename T>
    void SetHasModule();

    void SetHasModule(const u32 parModuleId);

    template<typename T>
    void RemoveModule();

    void RemoveModule(const u32 parModuleId);

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(FKey);
    }

    const BitSet<MaxModuleNumber>& GetKey() const;

private:
    BitSet<MaxModuleNumber> FKey;
};



} // namespace ECSEngine
