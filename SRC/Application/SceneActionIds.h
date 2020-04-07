#pragma once
#include "standalone/brigand.hpp"
namespace ECSEngine
{
enum class ESceneActionId
{
#define DECLARE_SCENE_ACTION(NAME) ESceneActionId_##NAME,
#include "SceneActionIds.inl"
#undef DECLARE_SCENE_ACTION
    Length
};

#define DECLARE_SCENE_ACTION(NAME) class NAME;
#include "SceneActionIds.inl"
#undef DECLARE_SCENE_ACTION

template<class T>
struct SceneActionTrait
{
    static constexpr u32 GetSceneActionTypeId()
    {
        AssertNotReached();
        return -1;
    }
};

#define DECLARE_SCENE_ACTION(NAME)                                                                                                                                                 \
    template<>                                                                                                                                                                     \
    struct SceneActionTrait<NAME>                                                                                                                                                  \
    {                                                                                                                                                                              \
        static constexpr u32 GetSceneActionTypeId() { return std::integral_constant<u32, static_cast<u32>(ESceneActionId::ESceneActionId_##NAME)>::value; }                        \
    };

#include "SceneActionIds.inl"
#undef DECLARE_SCENE_ACTION

} // namespace ECSEngine