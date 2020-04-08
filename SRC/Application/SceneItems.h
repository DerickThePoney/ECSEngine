#pragma once
#include "Common/PoolAllocator.h"
#include "Common/RefCountedObject.h"
#include "SceneItemsIds.h"

// TODO
// - POOL Allocation
// - Factories
namespace ECSEngine
{
#define DECLARE_SCENE_ITEM(TYPE)                                                                                                                                                   \
public:                                                                                                                                                                            \
    u32 GetSceneItemTypeId() const override { return SceneItemTraits<TYPE>::GetSceneItemTypeId(); }

class BaseSceneItem : public RefCountedObject
{
    DECLARE_POOL_ALLOCATED(BaseSceneItem);

public:
    BaseSceneItem();
    BaseSceneItem(const std::string& parName);

    virtual ~BaseSceneItem() {}

    virtual u32 GetSceneItemTypeId() const { return SceneItemTraits<BaseSceneItem>::GetSceneItemTypeId(); }

    void DrawEditor();

    bool ShouldShowItem() const { return FShowItem; }

    const std::string& GetName() const { return FName; }
    const glm::vec3& GetPosition() const { return FPosition; }
    const glm::quat& GetOrientation() const { return FOrientation; }

    void SetName(const std::string& parName) { FName = parName; }
    void SetPosition(const glm::vec3& parPosition) { FPosition = parPosition; }
    void SetOrientation(const glm::quat& parOrientation) { FOrientation = parOrientation; }

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(PROPERTY(Name), PROPERTY(Position), PROPERTY(Orientation));
    }

protected:
    virtual bool VirtualDrawEditor();

private:
    std::string FName;
    glm::vec3 FPosition;
    glm::quat FOrientation;

    bool FShowItem;

#ifdef PERFORM_SECURITY_CHECKS
    bool FVirtualDrawEditorHasBeenCalled;
#endif
};
} // namespace ECSEngine