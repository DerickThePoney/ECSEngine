#pragma once
#include "Application/SceneActions.h"

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
public:
    SpawnEntitySceneAction();
    virtual ~SpawnEntitySceneAction();

protected:
    virtual void VirtualInitialise() override;

    virtual void VirtualStart() override;

    virtual void VirtualDrawEditor() override;

public:
    const BaseSceneItem* GetSceneItem() const { return FSceneItem.get(); }
    void SetSceneItem(const BaseSceneItem* parSceneItem) { FSceneItem.reset(parSceneItem); }

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(cereal::base_class<ISceneAction>(this), PROPERTY(EntityTemplateName), PROPERTY(SceneItem));
    }

private:
    std::string FEntityTemplateName = "Entity template name";
    const EntityTemplate* FTemplate = nullptr;
    std::shared_ptr<const BaseSceneItem> FSceneItem = nullptr;
};
} // namespace ECSEngine