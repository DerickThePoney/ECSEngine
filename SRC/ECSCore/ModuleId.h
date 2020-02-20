#pragma once

namespace ECSEngine
{
enum class EModuleId
{
#define DECLARE_MODULE(NAME) EModuleId_##NAME,
#include "ModuleList.inl"
#undef DECLARE_MODULE
    Length
};

#define DECLARE_MODULE(NAME) class NAME;
#include "ModuleList.inl"
#undef DECLARE_MODULE

template<class T>
struct ModuleTraits
{
    static constexpr u32 GetModuleId()
    {
        AssertNotReached();
        return -1;
    }
};

#define DECLARE_MODULE(NAME)                                                                                                                                                       \
    template<>                                                                                                                                                                     \
    struct ModuleTraits<NAME>                                                                                                                                                      \
    {                                                                                                                                                                              \
        static constexpr u32 GetModuleId() { return std::integral_constant<u32, static_cast<u32>(EModuleId::EModuleId_##NAME)>::value; }                                           \
    };

#include "ModuleList.inl"
#undef DECLARE_MODULE
} // namespace ECSEngine