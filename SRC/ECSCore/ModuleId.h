#pragma once

namespace ECSEngine
{
enum class EModuleId
{
#define DECLARE_MODULE_AND_TEMPLATE(NAME, TEMPLATE) EModuleId_##NAME,
#include "ModuleList.inl"
#undef DECLARE_MODULE_AND_TEMPLATE
    Length
};

#define DECLARE_MODULE_AND_TEMPLATE(NAME, TEMPLATE) class NAME;
#include "ModuleList.inl"
#undef DECLARE_MODULE_AND_TEMPLATE

template<class T>
struct ModuleTraits
{
    static constexpr u32 GetModuleId()
    {
        AssertNotReached();
        return -1;
    }
};

#define DECLARE_MODULE_AND_TEMPLATE(NAME, TEMPLATE)                                                                                                                                \
    template<>                                                                                                                                                                     \
    struct ModuleTraits<NAME>                                                                                                                                                      \
    {                                                                                                                                                                              \
        static constexpr u32 GetModuleId() { return std::integral_constant<u32, static_cast<u32>(EModuleId::EModuleId_##NAME)>::value; }                                           \
    };

#include "ModuleList.inl"
#undef DECLARE_MODULE_AND_TEMPLATE
} // namespace ECSEngine