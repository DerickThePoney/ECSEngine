#include "stdafx.h"

#include "ModuleParameters.h"

namespace ECSEngine
{
namespace ModuleParameters
{
std::unordered_map<u32, std::unique_ptr<IParameterIdentifierTrait>> ParameterIdToIdentifierTrait;

void AddParameterIdentifierTrait(u32 parIdentifier, IParameterIdentifierTrait* parIdentifierTrait_Steal)
{
    ParameterIdToIdentifierTrait[parIdentifier].reset(parIdentifierTrait_Steal);
}

void RemoveParameterIdentifierTrait(u32 parIdentifier)
{
    ParameterIdToIdentifierTrait.erase(parIdentifier);
}

IParameterIdentifierTrait* GetIdentifierTrait(u32 parId)
{
    auto it = ParameterIdToIdentifierTrait.find(parId);
    if (it != ParameterIdToIdentifierTrait.end())
        return it->second.get();

    return nullptr;
}

void InitParameterIdentifiersTraits()
{
#define DECLARE_MODULE_PARAMETER(ID, TYPE) AddParameterIdentifierTrait(ID, new ParameterIdentifierTrait<ID>());
#include "ModuleParameters.inl"
#undef DECLARE_MODULE_PARAMETER
}

void DestroyParameterIdentifiersTraits()
{
#define DECLARE_MODULE_PARAMETER(ID, TYPE) RemoveParameterIdentifierTrait(ID);
#include "ModuleParameters.inl"
#undef DECLARE_MODULE_PARAMETER
}

} // namespace ModuleParameters
} // namespace ECSEngine
