#include "stdafx.h"

#include "PeonsHarvestingSystem.h"

#include "ColonyPeonsManagementModule.h"
#include "Common/TimeManager.h"
#include "ECSCore/AdjustableDebugParameters.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSGameplay_Common/EntityLinksModules.h"
#include "ECSGameplay_Common/MovementModule.h"
#include "ECSGameplay_Common/PathfindingManager.h"
#include "ECSGameplay_Common/PositionModule.h"
#include "ResourceHarvesterModule.h"
#include "ResourceStorageModule.h"

namespace ECSEngine
{
namespace
{
void GenerateNewPathfindRequest(const EntityId& parUnitId, const glm::vec3& parUnitPosition, const glm::vec3& parTargetPosition)
{
    PathfindingRequest request;
    request.UnitId = parUnitId;
    request.Start = glm::xz(parUnitPosition);
    request.End = glm::xz(parTargetPosition);

    Pathfinding::PushRequest(std::move(request));
}

void GoBackToColony(ResourceHarvesterModule& parPeonsHarvesterModule,
      ModuleAccessor<PositionModule>& parPeonsPositionAccessor,
      ModuleAccessor<PositionModule>& parColonyPositionAccessor,
      ModuleAccessor<MovementModule>& parPeonsMovementAccessor,
      ModuleAccessor<LinkToOwnerModule>& parPeonsOwnerAccessor)
{
    MovementModule* peonMovementModule = parPeonsMovementAccessor[parPeonsHarvesterModule.UnitId()];
    AssertRelease(peonMovementModule != nullptr);

    const PositionModule* peonPositionModule = parPeonsPositionAccessor[parPeonsHarvesterModule.UnitId()];
    AssertRelease(peonPositionModule != nullptr);

    const LinkToOwnerModule* peonOwnerModule = parPeonsOwnerAccessor[parPeonsHarvesterModule.UnitId()];
    AssertRelease(peonOwnerModule != nullptr);
    const EntityId& colonyId = peonOwnerModule->OwnerId();
    AssertRelease(colonyId.Valid());

    const PositionModule* colonyPositionModule = parColonyPositionAccessor[colonyId];
    AssertRelease(colonyPositionModule != nullptr);

    GenerateNewPathfindRequest(parPeonsHarvesterModule.UnitId(), peonPositionModule->GetPosition3D(), colonyPositionModule->GetPosition3D());
    peonMovementModule->SetRequestIsPending(true);

    parPeonsHarvesterModule.SetTarget(EntityId());
    parPeonsHarvesterModule.SetHarvesterState(HarvesterState::GOING_BACK_TO_COLONY);
}
} // namespace

PeonsHaverstingSystem::PeonsHaverstingSystem()
    : ModuleSystem()
{
    // Colony
    RegisterDepency<ColonyPeonsManagementModule>(Worlds::COLONY);
    RegisterDepency<PositionModule>(Worlds::COLONY);
    RegisterDepency<ResourceStorageModule>(Worlds::COLONY);

    // resource producers
    RegisterDepency<PositionModule>(Worlds::RESOURCE_PROD);
    RegisterDepency<ResourceStorageModule>(Worlds::RESOURCE_PROD);

    // peons
    RegisterDepency<PositionModule>(Worlds::PEONS);
    RegisterDepency<ResourceHarvesterModule>(Worlds::PEONS);
    RegisterDepency<ResourceStorageModule>(Worlds::PEONS);
    RegisterDepency<MovementModule>(Worlds::PEONS);
    RegisterDepency<LinkToOwnerModule>(Worlds::PEONS);
}

PeonsHaverstingSystem::~PeonsHaverstingSystem()
{
}

void PeonsHaverstingSystem::Debug()
{
    ADJUSTABLE_DEBUG_PARAMETER_BOOLEAN(showResources, false, "Show resources", "Storage");
    if (!showResources)
        return;

    LockControllers();

    static bool open = true;
    ImGui::Begin("Resources", &open, ImGuiWindowFlags_AlwaysAutoResize);

    if (ImGui::CollapsingHeader("Colonies"))
    {
        ModuleAccessor<ResourceStorageModule> colonyStorageAccessor(Worlds::COLONY);

        foreachitemconst(storage, colonyStorageAccessor)
        {
            ImGui::PushID(ImGui::GetID(&storage));
            if (ImGui::CollapsingHeader(fmt::format("Colony_{}", storage.UnitId().GetSequentialId()).c_str()))
            {
                forrange(i, 0, GameResource::LENGTH)
                {
                    const u32 resQ = storage.GetResourceQuantity((GameResource::Type)i);

                    if (resQ > 0)
                    {
                        ImGui::PushID(i);
                        ImGui::Text("%s : %d", GameResource::GetName((GameResource::Type)i), resQ);
                        ImGui::PopID();
                    }
                }
            }
            ImGui::PopID();
        }
    }

    if (ImGui::CollapsingHeader("Peons"))
    {
        ModuleAccessor<ResourceStorageModule> peonsStorageAccessor(Worlds::PEONS);

        foreachitemconst(storage, peonsStorageAccessor)
        {
            ImGui::PushID(ImGui::GetID(&storage));
            if (ImGui::CollapsingHeader(fmt::format("Peon_{}", storage.UnitId().GetSequentialId()).c_str()))
            {
                forrange(i, 0, GameResource::LENGTH)
                {
                    const u32 resQ = storage.GetResourceQuantity((GameResource::Type)i);

                    if (resQ > 0)
                    {
                        ImGui::PushID(i);
                        ImGui::Text("%s : %d", GameResource::GetName((GameResource::Type)i), resQ);
                        ImGui::PopID();
                    }
                }
            }
            ImGui::PopID();
        }
    }

    if (ImGui::CollapsingHeader("Producers"))
    {
        ModuleAccessor<ResourceStorageModule> producerStorageAccessor(Worlds::RESOURCE_PROD);

        foreachitemconst(storage, producerStorageAccessor)
        {
            ImGui::PushID(ImGui::GetID(&storage));
            if (ImGui::CollapsingHeader(fmt::format("Producer_{}", storage.UnitId().GetSequentialId()).c_str()))
            {
                forrange(i, 0, GameResource::LENGTH)
                {
                    const u32 resQ = storage.GetResourceQuantity((GameResource::Type)i);

                    if (resQ > 0)
                    {
                        ImGui::PushID(i);
                        ImGui::Text("%s : %d", GameResource::GetName((GameResource::Type)i), resQ);
                        ImGui::PopID();
                    }
                }
            }
            ImGui::PopID();
        }
    }

    ImGui::End();
    UnlockControllers();
}

void PeonsHaverstingSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<ColonyPeonsManagementModule> colonyPeonsModuleAccessor(Worlds::COLONY);

    ModuleAccessor<ResourceHarvesterModule> peonHarvesterAccessor(Worlds::PEONS);

    ModuleAccessor<PositionModule> peonPositionAccessor(Worlds::PEONS);
    ModuleAccessor<PositionModule> producerPositionAccessor(Worlds::RESOURCE_PROD);
    ModuleAccessor<PositionModule> colonyPositionAccessor(Worlds::COLONY);

    ModuleAccessor<LinkToOwnerModule> peonOwnerAccessor(Worlds::PEONS);

    ModuleAccessor<ResourceStorageModule> colonyStorageAccessor(Worlds::COLONY);
    ModuleAccessor<ResourceStorageModule> producerStorageAccessor(Worlds::RESOURCE_PROD);
    ModuleAccessor<ResourceStorageModule> peonsStorageAccessor(Worlds::PEONS);

    ModuleAccessor<MovementModule> peonsMovementAccessor(Worlds::PEONS);

    foreachitem(harvester, peonHarvesterAccessor)
    {
        const EntityId& peonId = harvester.UnitId();
        HarvesterState::Type currentState = harvester.HarvesterState();
        switch (currentState)
        {
        case ECSEngine::HarvesterState::IDLE:
        {
            if (harvester.Target().Valid())
            {
                harvester.SetHarvesterState(HarvesterState::GOING_TO_PRODUCER);
                MovementModule* peonMovementModule = peonsMovementAccessor[harvester.UnitId()];
                AssertRelease(peonMovementModule != nullptr);
                AssertRelease(!peonMovementModule->RequestIsPending());

                const PositionModule* peonPositionModule = peonPositionAccessor[peonId];
                AssertRelease(peonPositionModule != nullptr);

                const PositionModule* targetPositionModule = producerPositionAccessor[harvester.Target()];
                AssertRelease(targetPositionModule != nullptr);

                GenerateNewPathfindRequest(peonId, peonPositionModule->GetPosition3D(), targetPositionModule->GetPosition3D());
                peonMovementModule->SetRequestIsPending(true);
            }
            break;
        }
        case HarvesterState::GOING_TO_PRODUCER:
        {
            if (!harvester.Target().Valid())
            {
                GoBackToColony(harvester, peonPositionAccessor, colonyPositionAccessor, peonsMovementAccessor, peonOwnerAccessor);
                break;
            }

            // if no more resource in target, go back to colony for a new task
            const ResourceStorageModule* targetStorageModule = producerStorageAccessor[harvester.Target()];
            AssertRelease(targetStorageModule != nullptr);
            if (targetStorageModule->GetResourceQuantity(GameResource::LENGTH) == 0)
            {
                GoBackToColony(harvester, peonPositionAccessor, colonyPositionAccessor, peonsMovementAccessor, peonOwnerAccessor);
                break;
            }

            // on check le path restant
            MovementModule* peonMovementModule = peonsMovementAccessor[harvester.UnitId()];
            AssertRelease(peonMovementModule != nullptr);
            const std::vector<glm::vec2>& path = peonMovementModule->Path();
            u32 followedWaypoint = peonMovementModule->CurrentFollowedWayPoint();
            if (path.size() == 0 || followedWaypoint == -1)
            {
                harvester.SetRemainingHarvestingDuration(harvester.Template<ResourceHarvesterModuleTemplate>()->HarvestingDuration());
                harvester.SetHarvesterState(HarvesterState::HARVESTING);
            }
            break;
        }
        case HarvesterState::HARVESTING:
        {
            if (!harvester.Target().Valid())
            {
                GoBackToColony(harvester, peonPositionAccessor, colonyPositionAccessor, peonsMovementAccessor, peonOwnerAccessor);
                break;
            }

            // if no more resource in target, go back to colony for a new task
            ResourceStorageModule* targetStorageModule = producerStorageAccessor[harvester.Target()];
            AssertRelease(targetStorageModule != nullptr);
            if (targetStorageModule->GetResourceQuantity(GameResource::LENGTH) == 0)
            {
                GoBackToColony(harvester, peonPositionAccessor, colonyPositionAccessor, peonsMovementAccessor, peonOwnerAccessor);
                break;
            }

            const float currentRemainingTime = harvester.RemainingHarvestingDuration() - TimeManager::FrameDeltaTime();
            harvester.SetRemainingHarvestingDuration(currentRemainingTime);
            if (currentRemainingTime <= 0.f)
            {
                // Get the resource
                GameResource::Type mainResource = targetStorageModule->GetMainResource();
                const u32 retrieved = targetStorageModule->RemoveResource(mainResource, 1);

                ResourceStorageModule* peonStorage = peonsStorageAccessor[peonId];
                AssertRelease(peonStorage != nullptr);
                const u32 stored = peonStorage->AddResource(mainResource, retrieved);

                // Add back what you could not take on
                if (retrieved - stored > 0)
                    targetStorageModule->AddResource(mainResource, retrieved - stored);

                GoBackToColony(harvester, peonPositionAccessor, colonyPositionAccessor, peonsMovementAccessor, peonOwnerAccessor);
            }

            break;
        }
        case HarvesterState::GOING_BACK_TO_COLONY:
        {
            // on check le path restant
            MovementModule* peonMovementModule = peonsMovementAccessor[peonId];
            AssertRelease(peonMovementModule != nullptr);
            const std::vector<glm::vec2>& path = peonMovementModule->Path();
            u32 followedWaypoint = peonMovementModule->CurrentFollowedWayPoint();
            if (path.size() == 0 || followedWaypoint == -1)
            {
                const LinkToOwnerModule* peonOwnerModule = peonOwnerAccessor[peonId];
                AssertRelease(peonOwnerModule != nullptr);
                const EntityId& colonyId = peonOwnerModule->OwnerId();
                AssertRelease(colonyId.Valid());

                // Store back the resource
                ResourceStorageModule* peonStorage = peonsStorageAccessor[peonId];
                GameResource::Type mainResource = peonStorage->GetMainResource();
                AssertRelease(peonStorage != nullptr);
                const u32 retrieved = peonStorage->RemoveResource(mainResource, peonStorage->GetResourceQuantity(mainResource));

                ResourceStorageModule* colonyStorage = colonyStorageAccessor[colonyId];
                const u32 stored = colonyStorage->AddResource(mainResource, retrieved);

                // And the rest is lost
                harvester.SetHarvesterState(HarvesterState::IDLE);

                // On remet le peon en idle dans la colony
                ColonyPeonsManagementModule* colonyPeonsModule = colonyPeonsModuleAccessor[colonyId];
                AssertRelease(colonyPeonsModule != nullptr);
                colonyPeonsModule->SetPeonIdleIFN(peonId);
            }
            break;
        }
        default:
            AssertNotReached();
            break;
        }
    }
}

} // namespace ECSEngine