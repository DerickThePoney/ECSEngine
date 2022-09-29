#pragma once
#include "Common/PoolAllocator.h"
//

// TODO
// - POOL Allocation
// - Factories
namespace ECSEngine
{

class BaseSceneItem
{
    DECLARE_POOL_ALLOCATED(BaseSceneItem);

public:
    BaseSceneItem();
    BaseSceneItem(const std::string& parName, const u32 parId);

    virtual ~BaseSceneItem() {}

    virtual u32 GetSceneItemTypeId() const;

    void DrawEditor();

    bool ShouldShowItem() const { return FShowItem; }

    const std::string& GetName() const { return FName; }
    const vec3& GetPosition() const { return FPosition; }
    const vec3& GetEulerAngles() const { return FEulerAngles; }
    const u32 Id() const { return FId; }

    void SetName(const std::string& parName) { FName = parName; }
    void SetPosition(const vec3& parPosition) { FPosition = parPosition; }
    void SetEulerAngles(const vec3& parOrientation) { FEulerAngles = parOrientation; }

    bool ItemSelected() const { return FItemSelected; }
    bool ItemHovered() const { return FItemHovered; }
    void SetItemSelected(bool parIsSelected) { FItemSelected = parIsSelected; }
    void SetItemHovered(bool parIsHovered) { FItemHovered = parIsHovered; }

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(PROPERTY(Name), PROPERTY(Id), PROPERTY(Position), PROPERTY(EulerAngles));
    }

protected:
    virtual bool VirtualDrawEditor();

private:
    std::string FName;
    vec3 FPosition;
    vec3 FEulerAngles;

    u32 FId;
    bool FShowItem;

    bool FItemSelected;
    bool FItemHovered;

#ifdef PERFORM_SECURITY_CHECKS
    bool FVirtualDrawEditorHasBeenCalled;
#endif
};
} // namespace ECSEngine
