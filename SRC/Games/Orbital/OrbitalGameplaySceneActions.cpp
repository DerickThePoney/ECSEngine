#include "stdafx.h"

#include "OrbitalGameplaySceneActions.h"

#include "Application/SceneActionManagement.h"
#include "ECSCore/ECSCorePropertyDrawer.h"
#include "ECSCore/EntityFactory.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSGameplay_Common/ApparenceModule.h"
#include "OrbitalPathFollowerModule.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/GFXRepresentation.h"
#include "RenderingCore/GFXRepresentationDescriptor.h"
#include "RenderingCore/GFXRepresentationDescriptorManager.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/MeshManager.h"

CEREAL_REGISTER_TYPE(ECSEngine::CreateSolarSystemSceneAction);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::SceneActionWithBaseSceneItem, ECSEngine::CreateSolarSystemSceneAction)
namespace ECSEngine
{

IMPLEMENT_SCENE_ACTION(CreateSolarSystemSceneAction);

CreateSolarSystemSceneAction::CreateSolarSystemSceneAction(const std::string& parName)
    : parent_type(parName)
{
}

void CreateSolarSystemSceneAction::VirtualInitialise(const SceneScenario* parScene)
{
    parent_type::VirtualInitialise(parScene);
    FPlanetTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FPlanetTemplateName);
    FMoonTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FMoonTemplateName);
}

void CreateSolarSystemSceneAction::VirtualDrawEditor()
{
    parent_type::VirtualDrawEditor();

    if (ShouldShowEditor())
    {
        {
            const auto OldTemplate = FPlanetTemplateName;
            EDITOR_PROPERTY_ENTITY_TEMPLATE("Planet template", FPlanetTemplate, FPlanetTemplateName);
            if (FPlanetTemplateName != OldTemplate)
            {
                FPlanetTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FPlanetTemplateName);
            }
        }
        {
            const auto OldTemplate = FMoonTemplateName;
            EDITOR_PROPERTY_ENTITY_TEMPLATE("Moon template", FMoonTemplate, FMoonTemplateName);
            if (FMoonTemplateName != OldTemplate)
            {
                FMoonTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FMoonTemplateName);
            }
        }
    }
}

bool CreateSolarSystemSceneAction::VirtualDrawInSceneEditor(Rendering::DrawCommandBuffer& parCommandBuffer, Rendering::MaterialInstanceHandle& parMaterial)
{
    parent_type::VirtualDrawInSceneEditor(parCommandBuffer, parMaterial);

    if (FPlanetTemplate == nullptr || FMoonTemplate == nullptr)
        return false;

    const bool hasApparenceModule = FPlanetTemplate->HasModule<ApparenceModule>() && FMoonTemplate->HasModule<ApparenceModule>();
    if (!hasApparenceModule)
        return false;

    const bool hasOrbitalPath = FMoonTemplate->HasModule<OrbitalPathFollowerModule>();
    if (!hasOrbitalPath)
        return false;

    const BaseSceneItem* item = GetSceneItem();
    if (item == nullptr)
        return false;

    DrawTemplate(FPlanetTemplate, vec3{ }, item, parCommandBuffer, parMaterial);

    const OrbitalPathFollowerModuleTemplate* const PathFollowerTemplate = (OrbitalPathFollowerModuleTemplate*)FMoonTemplate->GetModuleTemplate<OrbitalPathFollowerModule>();
    AssertRelease(PathFollowerTemplate != nullptr);

    DrawTemplate(FMoonTemplate, PathFollowerTemplate->OrbitRadius() * vec3{ Cos(PathFollowerTemplate->OrbitPhase()), 0.f, Sin(PathFollowerTemplate->OrbitPhase()) }, item,
          parCommandBuffer, parMaterial);

    return true;
}

void CreateSolarSystemSceneAction::VirtualStart()
{
    parent_type::VirtualStart();

    AssertRelease(FSceneItem != nullptr);
    AssertRelease(FPlanetTemplate != nullptr);
    AssertRelease(FMoonTemplate != nullptr);

    ModuleParameters::ParameterContainer container;
    const vec3 PlanetPosition = FSceneItem->GetPosition();
    container.Set<ModuleParameters::Position>(FSceneItem->GetPosition());
    container.Set<ModuleParameters::EulerAnglesXYZ>(FSceneItem->GetEulerAngles());

    EntityId PlanetId = EntityFactory::RequestCreateEntity(FPlanetTemplate, container);

    const OrbitalPathFollowerModuleTemplate* const PathFollowerTemplate = (OrbitalPathFollowerModuleTemplate*)FMoonTemplate->GetModuleTemplate<OrbitalPathFollowerModule>();
    AssertRelease(PathFollowerTemplate != nullptr);

    container.Set<ModuleParameters::Position>(
          PlanetPosition + PathFollowerTemplate->OrbitRadius() * vec3{ Cos(PathFollowerTemplate->OrbitPhase()), 0.f, Sin(PathFollowerTemplate->OrbitPhase()) });
    container.Set<ModuleParameters::OwnerId>(PlanetId);

    EntityFactory::RequestCreateEntity(FMoonTemplate, container);

    Finish();
}

void CreateSolarSystemSceneAction::DrawTemplate(const EntityTemplate* Template,
      const vec3 Offset,
      const BaseSceneItem* Item,
      Rendering::DrawCommandBuffer& parCommandBuffer,
      Rendering::MaterialInstanceHandle& parMaterial) const
{
    const ApparenceModuleTemplate* apparenceModuleTemplate = (ApparenceModuleTemplate*)Template->GetModuleTemplate<ApparenceModule>();
    AssertRelease(apparenceModuleTemplate != nullptr);
    const Rendering::GFXRepresentationDescriptor* descriptor = Rendering::GFXRepresentationDescriptorManager::Instance().Descriptor(
          apparenceModuleTemplate->GFXRepresentationDescriptorName());
    AssertRelease(descriptor != nullptr);

    const Rendering::MeshHandle meshHandle = Rendering::MeshManager::Instance().CreateMesh(descriptor->MeshFile());
    if (meshHandle.IsValid())
    {
        const vec3 eulerAngles = Item->GetEulerAngles();
        const mat4 mtx = Translation(Item->GetPosition() + Offset) * EulerAnglesXYZ(eulerAngles.x, eulerAngles.y, eulerAngles.z);

        if (descriptor->IsMultiPassMaterial())
        {
            const Rendering::MultiPassMaterialInstanceHandle instanceHandle = Rendering::MaterialManager::CreateMultiPassMaterialInstanceIFN(descriptor->MaterialName());
            parCommandBuffer.DrawMesh(meshHandle, instanceHandle, mtx);
        }
        else
        {
            const Rendering::MaterialInstanceHandle instanceHandle = Rendering::MaterialManager::CreateMaterialInstanceIFN(descriptor->MaterialName());
            parCommandBuffer.DrawMesh(meshHandle, (instanceHandle.IsValid()) ? instanceHandle : parMaterial, mtx);
        }
    }
}

} // namespace ECSEngine
