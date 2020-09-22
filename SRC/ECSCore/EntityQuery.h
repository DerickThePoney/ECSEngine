#pragma once
#include "EntityFilter.h"

namespace ECSEngine
{

template<typename...>
struct EF
{
};

struct EmptyFilter
{
};

template<typename Required, typename Optional, typename Forbidden>
class EntityQuery;

template<typename... RequiredModules, typename... OptionalModules, typename... ForbiddenModules>
class EntityQuery<EF<RequiredModules...>, EF<OptionalModules...>, EF<ForbiddenModules...>>
{
public:
    EntityQuery() { }

private:
    EntityFilter<RequiredModules...> FRequiredEF;
    EntityFilter<OptionalModules...> FOptionalEF;
    EntityFilter<ForbiddenModules...> FForbiddenEF;
};
} // namespace ECSEngine