#pragma once

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

    // Drag
    float FLinearDamping = 0.1f;
    float FAngularDamping = 0.01f;

    // TO ADD:
    // - Colliders/PhysicsShapes
    // - Collision channels
    // - Drag (angular and linear)
    // - MassComputation
};
} // namespace Physics
} // namespace ECSEngine