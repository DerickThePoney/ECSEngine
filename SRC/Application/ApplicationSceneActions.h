#pragma once
#include "Common/PoolAllocator.h"
#include "SceneActions.h"

namespace ECSEngine
{
/*************************************************************/
/*            SceneActionWithBaseSceneItem                   */
/*************************************************************/

class BaseSceneItem;
class SceneActionWithBaseSceneItem : public ISceneAction
{
public:
    SceneActionWithBaseSceneItem(const std::string& parFName = "Dummy");
    virtual ~SceneActionWithBaseSceneItem();

    const BaseSceneItem* GetSceneItem() const { return FSceneItem; }

    void SetSceneItem(const BaseSceneItem* parSceneItem);

protected:
    virtual void VirtualInitialise(const Scene* parScene) override;

    virtual void VirtualStart() override;

    virtual void VirtualDrawEditor() override;

public:
    template<class Archive>
    void Serialize(Archive& ar)
    {
        ar(cereal::base_class<ISceneAction>(this), PROPERTY(SceneItemID));
    }

protected:
    const BaseSceneItem* FSceneItem = nullptr;
    u32 FSceneItemID = -1;
};
} // namespace ECSEngine