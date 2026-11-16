#include "stdafx.h"

#include "PlanetarySOIManager.h"

#include "ECSCore/ScopedModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/RigidbodyModule.h"
#include "OrbitalPathFollowerModule.h"
#include "Physics/PhysicsAPI.h"
#include "PlanetGravityModule.h"
#include "RenderingCore/DrawCommands.h"
#include "RocketManagerModule.h"

namespace ECSEngine
{

void PlanetarySOIManager::RegisterPlanet(const EntityId& PlanetId, const EntityId& ParentPlanet)
{
    AssertRelease(FEntityToNodeIdx.find(PlanetId) == FEntityToNodeIdx.end());
    if (ParentPlanet.Valid())
    {
        auto itFind = FEntityToNodeIdx.find(ParentPlanet);
        AssertRelease(itFind != FEntityToNodeIdx.end());
        PlanetNode& ParentNode = FPlanetNodes[itFind->second];

        ParentNode.FPlanetChildren.push_back(PlanetId);
    }

    PlanetNode newNode;
    newNode.FParentPlanetId = ParentPlanet;
    newNode.FPlanetId = PlanetId;
    FPlanetNodes.push_back(newNode);
    FEntityToNodeIdx[PlanetId] = FPlanetNodes.size() - 1;
}

using InitializeAccessHelpers = ScopedModuleAccessor<MC<PlanetGravityModule, EEntityWorlds::STANDARD>, MC<OrbitalPathFollowerModule, EEntityWorlds::STANDARD>>;
void PlanetarySOIManager::Initialize()
{
    if (FPlanetNodes.empty())
        return;

    AssertRelease(!FPlanetNodes[0].FParentPlanetId.Valid());

    InitializeAccessHelpers accessHelpers;

    foreachitem(node, FPlanetNodes)
    {
        if (node.FbSOIInitialised)
            continue;

        node.FbSOIInitialised = true;
        if (node.FParentPlanetId.Valid())
        {
            const PlanetGravityModule* ParentPlanetGravityModule = accessHelpers.GetModule<PlanetGravityModule>(node.FParentPlanetId);
            AssertRelease(ParentPlanetGravityModule != nullptr);

            const PlanetGravityModule* ThisPlanetGravityModule = accessHelpers.GetModule<PlanetGravityModule>(node.FPlanetId);
            AssertRelease(ThisPlanetGravityModule != nullptr);

            const OrbitalPathFollowerModule* ThisPlanetOrbitFollowerModule = accessHelpers.GetModule<OrbitalPathFollowerModule>(node.FPlanetId);
            AssertRelease(ThisPlanetOrbitFollowerModule != nullptr);

            node.FSOIRadius = ThisPlanetOrbitFollowerModule->OrbitRadius() *
                  pow((ThisPlanetGravityModule->PlanetGravityConstant() / ParentPlanetGravityModule->PlanetGravityConstant()), 2.f / 5.f);
        }
    }

    FbInitialized = true;
}

using UpdateAccessHelpers =
      ScopedModuleAccessor<MC<PlanetGravityModule, EEntityWorlds::STANDARD>, MC<RigidbodyModule, EEntityWorlds::STANDARD>, MC<RocketManagerModule, EEntityWorlds::STANDARD>>;

void PlanetarySOIManager::Update()
{
    if (!FbInitialized)
    {
        Initialize();
        if (!FbInitialized)
            return;
    }

    UpdateAccessHelpers accessHelpers;

    AssertRelease(accessHelpers.Accessor<RocketManagerModule>().size() <= 1);
    if (accessHelpers.Accessor<RocketManagerModule>().size() == 0)
        return;

    RocketManagerModule& rocketManager = *accessHelpers.Accessor<RocketManagerModule>().begin();
    vec3 RocketPosition;
    rocketManager.ComputeCenterOfMass(accessHelpers.Accessor<RigidbodyModule>(), RocketPosition);

    EntityId RocketGravity = rocketManager.CurrentGravityTarget();
    if (!RocketGravity.Valid())
    {
        RocketGravity = FPlanetNodes[0].FPlanetId;
    }

    bool bShouldContinueSearching = true;

    std::queue<EntityId> targetsToSearch;
    targetsToSearch.push(RocketGravity);
    while (!targetsToSearch.empty())
    {
        EntityId CurrentTarget = targetsToSearch.front();
        targetsToSearch.pop();

        AssertRelease(FEntityToNodeIdx.find(CurrentTarget) != FEntityToNodeIdx.end());
        const PlanetNode& targetNode = FPlanetNodes[FEntityToNodeIdx[CurrentTarget]];

        if (targetNode.FParentPlanetId.Valid())
        {
            // Finite SOI, need to check that we're still in
            RigidbodyModule* targetBody = accessHelpers.GetModule<RigidbodyModule>(CurrentTarget);
            AssertRelease(targetBody != nullptr);

            vec3 TargetPosition;
            Physics::GetBodyCenterOfMass(targetBody->BodyHandle(), TargetPosition);

            const float DistanceToTargetSq = LengthSq(RocketPosition - TargetPosition);

            const float stayingInRadius = 1.02f * targetNode.FSOIRadius;
            if (DistanceToTargetSq > stayingInRadius * stayingInRadius)
            {
                RocketGravity = targetNode.FParentPlanetId;
                break;
            }

            if (DistanceToTargetSq <= targetNode.FSOIRadius * targetNode.FSOIRadius)
            {
                RocketGravity = CurrentTarget;
            }
        }

        foreachitem(childPlanet, targetNode.FPlanetChildren)
        {
            targetsToSearch.push(childPlanet);
        }
    }

    rocketManager.SetCurrentGravityTarget(RocketGravity);
}

void PlanetarySOIManager::DrawDebug(Rendering::DrawCommandBuffer* buffer, const Rendering::MaterialInstanceHandle& material)
{
    if (!FbInitialized)
        return;

    using DrawAccessHelpers = ScopedModuleAccessor<MC<RigidbodyModule, EEntityWorlds::STANDARD>>;
    DrawAccessHelpers accessHelpers;

    foreachitem(node, FPlanetNodes)
    {
        if (node.FSOIRadius <= 0.f)
            continue;

        const RigidbodyModule* body = accessHelpers.GetModule<RigidbodyModule>(node.FPlanetId);
        if (body == nullptr)
            continue;

        vec3 centerOfMass;
        if (!Physics::GetBodyCenterOfMass(body->BodyHandle(), centerOfMass))
            continue;

        buffer->DrawDebugSphere(material, centerOfMass, node.FSOIRadius, 0xFFFF8800u);
    }
}

} // namespace ECSEngine