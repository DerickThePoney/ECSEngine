#pragma once

#define DONOTHING (void)(0)

namespace ECSEngine
{
namespace ForEach
{
template<typename T>
struct ReverseWrapper
{
    T FContainer;
};

template<typename T>
struct ReverseWrapper<T&>
{
    T& FContainer;
};

template<typename T>
auto begin(const ReverseWrapper<T>& w)
{
    return w.FContainer.rbegin();
}

template<typename T>
auto end(const ReverseWrapper<T>& w)
{
    return w.FContainer.rend();
}

template<typename T>
ReverseWrapper<T> reverse(T&& container)
{
    return { std::forward<T>(container) };
}

} // namespace ForEach
} // namespace ECSEngine

#define forrange(VAR, START, END) for (size_t VAR = START; VAR < END; ++VAR)
#define reverseforrange(VAR, START, END) for (size_t VAR = END - 1, STOP = END; STOP != START; --VAR, --STOP)

#define foreachitem(VAR, CONTAINER) for (auto& VAR : CONTAINER)

#define foreachitemconst(VAR, CONTAINER) for (const auto& VAR : CONTAINER)

#define reverseforeachitem(VAR, CONTAINER) for (auto& VAR : ECSEngine::ForEach::reverse(CONTAINER))
#define reverseforeachitemconst(VAR, CONTAINER) for (const auto& VAR : ECSEngine::ForEach::reverse(CONTAINER))

#ifdef PERFORM_SECURITY_CHECKS
#define ENABLE_DEBUG_PARAMETERS

#endif