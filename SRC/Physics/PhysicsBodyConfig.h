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
    CollisionShape FShape;

    // Drag
    float FLinearDamping = 0.1f;
    float FAngularDamping = 0.01f;

    // Moveability
    EPhysicsMoveability::Type FMoveability = EPhysicsMoveability::STATIC;

    // TO ADD:
    // - Colliders/PhysicsShapes
    // - Collision channels
    // - Drag (angular and linear)
    // - MassComputation
    // - Proper CoM computation
    // - Moveability

    SERIALIZE()
    {
        PROPERTYFIELD(ApplyGravity, true);
        PROPERTYFIELD(AutoComputeMass, false);
        PROPERTYFIELD(Mass, 0.f);
        PROPERTYFIELD(Density, 1.f);
        PROPERTYFIELD(Shape, CollisionShape());
        PROPERTYFIELD(LinearDamping, 0.1f);
        PROPERTYFIELD(AngularDamping, 0.01f);
        PROPERTYFIELD(Moveability, EPhysicsMoveability::STATIC);
    }

    void DrawInEditor();
};
} // namespace Physics
} // namespace ECSEngine