#pragma once
#include "ECSGameplay_Common/GameplaySceneActions.h"
#include "WorldGenerationParameters.h"

namespace ECSEngine
{
/**********************************************************************
 * CreateWorldSceneAction: Creates the world for the game.
 *      1- Places resources
 *      2- Places start positions for players
 *      3- Creates the world static geometry
 **********************************************************************/
class CreateWorldSceneAction : public ISceneAction
{
    DECLARE_SCENE_ACTION(CreateWorldSceneAction, ISceneAction);

public:
    CreateWorldSceneAction(const std::string& parName = "Dummy");
    virtual ~CreateWorldSceneAction() { }

protected:
    virtual void VirtualInitialise(const SceneScenario* parScene) override;

    virtual void VirtualShutdown() override;

    virtual void VirtualDrawEditor() override;

    virtual bool VirtualDrawInSceneEditor(Rendering::DrawCommandBuffer& parCommandBuffer, Rendering::MaterialInstanceHandle& parMaterial) override;

    virtual void VirtualStart() override;

    virtual void VirtualUpdate() override;

    virtual void VirtualFinish() override;

public:
    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(cereal::base_class<parent_type>(this));
        PROPERTYFIELD(WorldParameters, WorldGenerationParameters());
    }

private:
    WorldGenerationParameters FWorldParameters;

    const EntityTemplate* FFirePlaceTemplate = nullptr;
};
} // namespace ECSEngine
