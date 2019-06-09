#pragma once
#include "ModuleId.h"

namespace ECSEngine
{
class Module
{
public:
protected:
    Module() {}
    ~Module() {}

private:
};

#define DECLARE_MODULE(TYPE)                                                                                                                                                       \
public:                                                                                                                                                                            \
    constexpr u32 GetModuleId() const { return ModuleTraits<TYPE>::GetModuleId(); }
} // namespace ECSEngine