#include "stdafx.h"

#include "CollisionShape.h"

#include "Application/PropertyDrawer.h"
#include "GeometryHelpers.h"

namespace ECSEngine
{
namespace Physics
{
std::string GetName(ECollisionShape shapeType)
{
    switch (shapeType)
    {
    case ECSEngine::Physics::ECollisionShape::NONE:
        return "NONE";
        break;
    case ECSEngine::Physics::ECollisionShape::BOX:
        return "BOX";
        break;
    case ECSEngine::Physics::ECollisionShape::SPHERE:
        return "SPHERE";
        break;
    default:
        break;
    }

    return "GetName ECOLLISIONSHAPE ERROR";
}

IMPLEMENT_POOL_ALLOCATED(CollisionShape);

CollisionShape CollisionShape::MakeSphere(vec3 Center, float Radius)
{
    CollisionShape shape;
    shape.FShapeType = ECollisionShape::SPHERE;
    shape.FCenter = Center;
    shape.FShapeData.FSphereData.Radius = Radius;
    return shape;
}

CollisionShape CollisionShape::MakeBox(vec3 Center, vec3 Extents)
{
    CollisionShape shape;
    shape.FShapeType = ECollisionShape::BOX;
    shape.FCenter = Center;
    shape.FShapeData.FBoxData.FExtents = Extents;
    return shape;
}

float CollisionShape::ComputeMass(float Density) const
{
    switch (FShapeType)
    {
    case ECollisionShape::BOX:
        return Density * FShapeData.FBoxData.FExtents.x * FShapeData.FBoxData.FExtents.y * FShapeData.FBoxData.FExtents.z;
    case ECollisionShape::SPHERE:
        return 2.f * Pi() * FShapeData.FSphereData.Radius * Density;
    }
    return 0.f;
}

mat3 CollisionShape::ComputeInertiaTensor(float Mass) const
{
    mat3 inertiaTensor = mat3::Identity();
    switch (FShapeType)
    {
    case ECollisionShape::BOX:
    {
        inertiaTensor.FValues[0] = 1 / 12.f * Mass *
              (FShapeData.FBoxData.FExtents.y * FShapeData.FBoxData.FExtents.y + FShapeData.FBoxData.FExtents.z * FShapeData.FBoxData.FExtents.z);
        inertiaTensor.FValues[4] = 1 / 12.f * Mass *
              (FShapeData.FBoxData.FExtents.x * FShapeData.FBoxData.FExtents.x + FShapeData.FBoxData.FExtents.z * FShapeData.FBoxData.FExtents.z);
        inertiaTensor.FValues[8] = 1 / 12.f * Mass *
              (FShapeData.FBoxData.FExtents.x * FShapeData.FBoxData.FExtents.x + FShapeData.FBoxData.FExtents.y * FShapeData.FBoxData.FExtents.y);
    }
    case ECollisionShape::SPHERE:
    {
        inertiaTensor *= 2.f / 5.f * Mass * FShapeData.FSphereData.Radius;
    }
    }
    return inertiaTensor;
}

AABB3f CollisionShape::ComputeAABB(const mat4& Transform) const
{
    mat3 inertiaTensor = mat3::Identity();
    switch (FShapeType)
    {
    case ECollisionShape::BOX:
    {
        AABB3f OBB(FCenter - 0.5f * FShapeData.FBoxData.FExtents, FCenter + 0.5f * FShapeData.FBoxData.FExtents);
        return GeometryHelpers::ComputeAABBFromOBB(OBB, Transform);
    }
    case ECollisionShape::SPHERE:
    {
        AABB3f OBB(FCenter - vec3(FShapeData.FSphereData.Radius), FCenter + vec3(FShapeData.FSphereData.Radius));
        return GeometryHelpers::ComputeAABBFromOBB(OBB, Transform);
    }
    }

    return AABB3f();
}

void CollisionShape::DrawInEditor()
{
    if (ImGui::CollapsingHeader("CollisionShape"))
    {
        ImGui::Indent();
        if (ImGui::BeginCombo("Shape Type", GetName(FShapeType).c_str()))
        {
            {
                bool is_selected = (FShapeType == ECollisionShape::NONE);
                if (ImGui::Selectable(GetName(ECollisionShape::NONE).c_str(), is_selected))
                    FShapeType = ECollisionShape::NONE;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }

            {
                bool is_selected = (FShapeType == ECollisionShape::BOX);
                if (ImGui::Selectable(GetName(ECollisionShape::BOX).c_str(), is_selected))
                    FShapeType = ECollisionShape::BOX;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }

            {
                bool is_selected = (FShapeType == ECollisionShape::SPHERE);
                if (ImGui::Selectable(GetName(ECollisionShape::SPHERE).c_str(), is_selected))
                    FShapeType = ECollisionShape::SPHERE;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        EDITOR_PROPERTY_SIMPLE("Offset", FCenter);

        switch (FShapeType)
        {
        case ECSEngine::Physics::ECollisionShape::BOX:
            EDITOR_PROPERTY_WITH_LIMITS("Extents", FShapeData.FBoxData.FExtents, vec3(0.f), vec3(10000.f));
            break;
        case ECSEngine::Physics::ECollisionShape::SPHERE:
            EDITOR_PROPERTY_WITH_LIMITS("Radius", FShapeData.FSphereData.Radius, 0.f, 10000.f);
            break;
        default:
            break;
        }
        ImGui::Unindent();
    }
}

} // namespace Physics
} // namespace ECSEngine
