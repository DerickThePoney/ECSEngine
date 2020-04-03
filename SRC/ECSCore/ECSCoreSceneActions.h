#pragma once
#include "Application/SceneActions.h"
#include "Common/PoolAllocator.h"

namespace ECSEngine
{
/*************************************************************/
/*            SpawnEntitySceneAction                         */
/*************************************************************/
// TODO:
//    - Ajouter les membres de classe (en gros template + BaseSceneItem ref) + serialization
//    - Initialise -> Vérifier que le template existe
//    - Start -> spawn + finish

class EntityTemplate;
class BaseSceneItem;
class SpawnEntitySceneAction : public ISceneAction
{
    DECLARE_POOL_ALLOCATED(SpawnEntitySceneAction);

public:
    SpawnEntitySceneAction(const std::string& parFName = "Dummy");
    virtual ~SpawnEntitySceneAction();

protected:
    virtual void VirtualInitialise() override;

    virtual void VirtualStart() override;

    virtual void VirtualDrawEditor() override;

public:
    const BaseSceneItem* GetSceneItem() const { return FSceneItem; }
    const std::string& GetEntityTemplateName() const { return FEntityTemplateName; }

    void SetEntityTemplateName(const std::string& parName) { FEntityTemplateName = parName; }
    void SetSceneItem(const BaseSceneItem* parSceneItem) { FSceneItem = parSceneItem; }

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(cereal::base_class<ISceneAction>(this), PROPERTY(EntityTemplateName), PROPERTY(SceneItem));
    }

private:
    std::string FEntityTemplateName = "Entity template name";
    const EntityTemplate* FTemplate = nullptr;
    const BaseSceneItem* FSceneItem = nullptr;
};
} // namespace ECSEngine