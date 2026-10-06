#pragma once
#include "CollisionShape.h"
#include "PhysicsMoveabilityEnum.h"

namespace ECSEngine
{
namespace Physics
{
struct PhysicsBodyConfig
{
    // TODO CREATE CATEGORIES AND REORDER THIS IN SEVERAL STRUCTS
    bool FApplyGravity = true;

    bool FAutoComputeMass = false;
    float FMass = 0.f;
    float FDensity = 1.f;
    std::vector<CollisionShape> FShapes;

    // Drag
    float FLinearDamping = 0.1f;
    float FAngularDamping = 0.01f;

    // Moveability
    EPhysicsMoveability::Type FMoveability = EPhysicsMoveability::STATIC;

    // Physics material
    float FRestitution = 0.2f;
    float FFriction = 0.4f;

    // TO ADD:
    // - Collision channels
    // - Drag (angular and linear)

    SERIALIZE()
    {
        PROPERTYFIELD(ApplyGravity, true);
        PROPERTYFIELD(AutoComputeMass, false);
        PROPERTYFIELD(Mass, 0.f);
        PROPERTYFIELD(Density, 1.f);
        PROPERTYFIELD(Shapes, std::vector<CollisionShape>());
        PROPERTYFIELD(LinearDamping, 0.1f);
        PROPERTYFIELD(AngularDamping, 0.01f);
        PROPERTYFIELD(Moveability, EPhysicsMoveability::STATIC);
        PROPERTYFIELD(Restitution, 0.2f);
        PROPERTYFIELD(Friction, 0.4f);
    }

    void DrawInEditor();
};
} // namespace Physics
} // namespace ECSEngine