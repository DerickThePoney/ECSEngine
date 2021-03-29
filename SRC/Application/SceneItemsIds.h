#pragma once
namespace ECSEngine
{
enum class ESceneItemId
{
#define DECLARE_SCENE_ITEM(NAME) ESceneItemId_##NAME,
#include "SceneItemsId.inl"
#undef DECLARE_SCENE_ITEM
    Length
};

#define DECLARE_SCENE_ITEM(NAME) class NAME;
#include "SceneItemsId.inl"
#undef DECLARE_SCENE_ITEM

template<class T>
struct SceneItemTraits
{
    static constexpr u32 GetSceneItemTypeId()
    {
        AssertNotReached();
        return -1;
    }
};

#define DECLARE_SCENE_ITEM(NAME)                                                                                                                                                   \
    template<>                                                                                                                                                                     \
    struct SceneItemTraits<NAME>                                                                                                                                                   \
    {                                                                                                                                                                              \
        static constexpr u32 GetSceneItemTypeId() { return std::integral_constant<u32, static_cast<u32>(ESceneItemId::ESceneItemId_##NAME)>::value; }                              \
    };

#include "SceneItemsId.inl"
#undef DECLARE_SCENE_ITEM

} // namespace ECSEngine
