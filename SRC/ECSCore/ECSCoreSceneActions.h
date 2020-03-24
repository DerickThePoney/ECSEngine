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
class SpawnEntitySceneAction : public ISceneAction
{
public:
    SpawnEntitySceneAction();
    virtual ~SpawnEntitySceneAction();

protected:
    virtual void VirtualInitialise() override;

    virtual void VirtualStart() override;

private:
};
} // namespace ECSEngine