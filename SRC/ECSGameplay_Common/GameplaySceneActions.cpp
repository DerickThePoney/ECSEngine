#include "stdafx.h"

#include "GameplaySceneActions.h"

#include "ApparenceModule.h"
#include "Application/SceneActionManagement.h"
#include "Application/SceneItems.h"
#include "Common/NavMeshPathSmoother.h"
#include "Common/NavMeshPathSolver.h"
#include "Common/NavMeshSolver.h"
#include "Common/PolygonRandomGenerator.h"
#include "ECSCore/ECSCorePropertyDrawer.h"
#include "ECSCore/EntityFactory.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleParameters.h"
#include "NavMeshPathfindingManager.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/GFXRepresentation.h"
#include "RenderingCore/GFXRepresentationDescriptor.h"
#include "RenderingCore/GFXRepresentationDescriptorManager.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/MeshManager.h"

CEREAL_REGISTER_TYPE(ECSEngine::SpawnEntitySceneAction);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::SceneActionWithBaseSceneItem, ECSEngine::SpawnEntitySceneAction)

CEREAL_REGISTER_TYPE(ECSEngine::SpawnEntitiesInPolygonalPatternSceneAction);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::SceneActionPolygonalPattern, ECSEngine::SpawnEntitiesInPolygonalPatternSceneAction)

CEREAL_REGISTER_TYPE(ECSEngine::CreateNavMeshSceneAction);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::SceneActionPolygonalPattern, ECSEngine::CreateNavMeshSceneAction)

namespace ECSEngine
{

/*************************************************************/
/*                 SpawnEntitySceneAction                    */
/*************************************************************/
IMPLEMENT_SCENE_ACTION(SpawnEntitySceneAction);

SpawnEntitySceneAction::SpawnEntitySceneAction(const std::string& parFName)
    : parent_type(parFName)
{
}

SpawnEntitySceneAction::~SpawnEntitySceneAction()
{
}

void SpawnEntitySceneAction::VirtualInitialise(const SceneScenario* parScene)
{
    parent_type::VirtualInitialise(parScene);
    FTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FEntityTemplateName);
}

void SpawnEntitySceneAction::VirtualStart()
{
    parent_type::VirtualStart();

    AssertRelease(FSceneItem != nullptr);
    AssertRelease(FTemplate != nullptr);

    ModuleParameters::ParameterContainer container;
    container.Set<ModuleParameters::Position>(FSceneItem->GetPosition());
    container.Set<ModuleParameters::EulerAngles>(FSceneItem->GetEulerAngles());

    EntityFactory::CreateEntity(FTemplate, container);

    Finish();
}

void SpawnEntitySceneAction::VirtualDrawEditor()
{
    parent_type::VirtualDrawEditor();

    if (ShouldShowEditor())
    {
        EDITOR_PROPERTY_ENTITY_TEMPLATE("Entity to spawn", FTemplate, FEntityTemplateName);
    }
}

bool SpawnEntitySceneAction::VirtualDrawInSceneEditor(Rendering::DrawCommandBuffer& parCommandBuffer, Rendering::MaterialInstanceHandle& parMaterial)
{
    parent_type::VirtualDrawInSceneEditor(parCommandBuffer, parMaterial);
    if (FTemplate == nullptr)
        return false;

    const bool hasApparenceModule = FTemplate->HasModule<ApparenceModule>();
    if (!hasApparenceModule)
        return false;

    const BaseSceneItem* item = GetSceneItem();
    if (item == nullptr)
        return false;

    const ApparenceModuleTemplate* apparenceModuleTemplate = (ApparenceModuleTemplate*)FTemplate->GetModuleTemplate<ApparenceModule>();
    AssertRelease(apparenceModuleTemplate != nullptr);
    const Rendering::GFXRepresentationDescriptor* descriptor = Rendering::GFXRepresentationDescriptorManager::Instance().Descriptor(
          apparenceModuleTemplate->GFXRepresentationDescriptorName());
    AssertRelease(descriptor != nullptr);

    const Rendering::MeshHandle meshHandle = Rendering::MeshManager::Instance().CreateMesh(descriptor->MeshFile());
    const Rendering::MaterialInstanceHandle instanceHandle = Rendering::MaterialManager::CreateMaterialInstanceIFN(descriptor->MaterialName());
    if (meshHandle.IsValid())
    {
        const glm::vec3 eulerAngles = item->GetEulerAngles();
        const glm::mat4 mtx = glm::translate(item->GetPosition()) * glm::eulerAngleXYZ(eulerAngles.x, eulerAngles.y, eulerAngles.z);

        parCommandBuffer.DrawMesh(meshHandle, (instanceHandle.IsValid()) ? instanceHandle : parMaterial, mtx);
    }

    return true;
}

/*************************************************************/
/*        SpawnEntitiesInPolygonalPatternSceneAction         */
/*************************************************************/
IMPLEMENT_SCENE_ACTION(SpawnEntitiesInPolygonalPatternSceneAction);

SpawnEntitiesInPolygonalPatternSceneAction::SpawnEntitiesInPolygonalPatternSceneAction(const std::string& parFName /*= "Dummy"*/)
    : parent_type(parFName)
{
}

SpawnEntitiesInPolygonalPatternSceneAction::~SpawnEntitiesInPolygonalPatternSceneAction()
{
}

void SpawnEntitiesInPolygonalPatternSceneAction::VirtualInitialise(const SceneScenario* parScene)
{
    parent_type::VirtualInitialise(parScene);
    FTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FEntityTemplateName);
}

void SpawnEntitiesInPolygonalPatternSceneAction::VirtualStart()
{
    parent_type::VirtualStart();

    AssertRelease(FSceneItem != nullptr);
    AssertRelease(FTemplate != nullptr);

    if (FRandomPoints.size() == 0)
        GenerateRandomPoints();

    const glm::vec3 position = FSceneItem->GetPosition();
    const glm::vec3 eulerAngles = FSceneItem->GetEulerAngles();
    forrange(i, 0, FRandomPoints.size())
    {
        ModuleParameters::ParameterContainer container;
        container.Set<ModuleParameters::Position>(position + glm::vec3(FRandomPoints[i].x, 0.f, FRandomPoints[i].y));
        container.Set<ModuleParameters::EulerAngles>(FSceneItem->GetEulerAngles());

        EntityFactory::CreateEntity(FTemplate, container);
    }

    Finish();
}

void SpawnEntitiesInPolygonalPatternSceneAction::VirtualDrawEditor()
{
    parent_type::VirtualDrawEditor();

    if (ShouldShowEditor())
    {
        EDITOR_PROPERTY_ENTITY_TEMPLATE("Entity to spawn", FTemplate, FEntityTemplateName);
        EDITOR_PROPERTY_WITH_LIMITS("Number of entities", FNumberOfEntities, 1u, 1000u);

        ImGui::Checkbox("Show entities in editor", &FShowEntitiesInEditor);

        if (ImGui::Button("Regenerate"))
        {
            GenerateRandomPoints();
        }
    }
}

bool SpawnEntitiesInPolygonalPatternSceneAction::VirtualDrawInSceneEditor(Rendering::DrawCommandBuffer& parCommandBuffer, Rendering::MaterialInstanceHandle& parMaterial)
{
    parent_type::VirtualDrawInSceneEditor(parCommandBuffer, parMaterial);

    if (!FShowEntitiesInEditor)
        return false;

    if (FTemplate == nullptr)
        return false;

    const bool hasApparenceModule = FTemplate->HasModule<ApparenceModule>();
    if (!hasApparenceModule)
        return false;

    if (FRandomPoints.size() == 0)
        GenerateRandomPoints();

    const BaseSceneItem* item = GetSceneItem();
    if (item == nullptr)
        return false;

    const ApparenceModuleTemplate* apparenceModuleTemplate = (ApparenceModuleTemplate*)FTemplate->GetModuleTemplate<ApparenceModule>();
    AssertRelease(apparenceModuleTemplate != nullptr);
    const Rendering::GFXRepresentationDescriptor* descriptor = Rendering::GFXRepresentationDescriptorManager::Instance().Descriptor(
          apparenceModuleTemplate->GFXRepresentationDescriptorName());
    AssertRelease(descriptor != nullptr);

    const Rendering::MeshHandle meshHandle = Rendering::MeshManager::Instance().CreateMesh(descriptor->MeshFile());
    const Rendering::MaterialInstanceHandle instanceHandle = Rendering::MaterialManager::CreateMaterialInstanceIFN(descriptor->MaterialName());
    if (meshHandle.IsValid())
    {
        const glm::vec3 offset = (GetSceneItem() == nullptr) ? glm::vec3(0.f) : GetSceneItem()->GetPosition();
        foreachitemconst(point, FRandomPoints)
        {
            const glm::vec3 eulerAngles = item->GetEulerAngles();
            const glm::mat4 mtx = glm::translate(item->GetPosition() + glm::vec3(point.x, 0.f, point.y)) * glm::eulerAngleXYZ(eulerAngles.x, eulerAngles.y, eulerAngles.z);
            parCommandBuffer.DrawMesh(meshHandle, (instanceHandle.IsValid()) ? instanceHandle : parMaterial, mtx);
        }
    }

    return true;
}

void SpawnEntitiesInPolygonalPatternSceneAction::GenerateRandomPoints()
{
    PolygonRandomGenerator randomGenerator;
    RandomPolygonGenerationParameters params = { FNumberOfEntities };

    FRandomPoints.clear();
    FRandomPoints.reserve(params.NumberOfPoints);

    randomGenerator.GenerateRandomPoints(Polygon(), PolygonHoles(), params, FRandomPoints);
}

/*******************************************/
/*        CreateNavMeshSceneAction         */
/*******************************************/
IMPLEMENT_SCENE_ACTION(CreateNavMeshSceneAction);
CreateNavMeshSceneAction::CreateNavMeshSceneAction(const std::string& parFName /*= "Dummy"*/)
    : parent_type(parFName)
{
}

void CreateNavMeshSceneAction::VirtualInitialise(const SceneScenario* parScene)
{
    parent_type::VirtualInitialise(parScene);
    Pathfinding::InitialisePathfinder(Polygon(), PolygonHoles());
}

void CreateNavMeshSceneAction::VirtualStart()
{
    parent_type::VirtualStart();
    Finish();
}

void CreateNavMeshSceneAction::VirtualDrawEditor()
{
    parent_type::VirtualDrawEditor();
}

bool CreateNavMeshSceneAction::VirtualDrawInSceneEditor(Rendering::DrawCommandBuffer& parCommandBuffer, Rendering::MaterialInstanceHandle& parMaterial)
{
    return parent_type::VirtualDrawInSceneEditor(parCommandBuffer, parMaterial);
}
} // namespace ECSEngine
