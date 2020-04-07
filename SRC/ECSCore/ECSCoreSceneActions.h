#pragma once
#include "Application/SceneActions.h"
#include "Common/PoolAllocator.h"

namespace ECSEngine
{
/*************************************************************/
/*            SpawnEntitySceneAction                         */
/*************************************************************/

class EntityTemplate;
class BaseSceneItem;
class SpawnEntitySceneAction : public ISceneAction
{
    DECLARE_SCENE_ACTION(SpawnEntitySceneAction);

public:
    SpawnEntitySceneAction(const std::string& parFName = "Dummy");
    virtual ~SpawnEntitySceneAction();

protected:
    virtual void VirtualInitialise(const Scene* parScene) override;

    virtual void VirtualStart() override;

    virtual void VirtualDrawEditor() override;

public:
    const BaseSceneItem* GetSceneItem() const { return FSceneItem; }
    const std::string& GetEntityTemplateName() const { return FEntityTemplateName; }

    void SetEntityTemplateName(const std::string& parName) { FEntityTemplateName = parName; }
    void SetSceneItem(const BaseSceneItem* parSceneItem);

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(cereal::base_class<ISceneAction>(this), PROPERTY(EntityTemplateName), PROPERTY(SceneItemID));
    }

private:
    std::string FEntityTemplateName = "Entity template name";
    u32 FSceneItemID = -1;
    const EntityTemplate* FTemplate = nullptr;
    const BaseSceneItem* FSceneItem = nullptr;
};
} // namespace ECSEngine