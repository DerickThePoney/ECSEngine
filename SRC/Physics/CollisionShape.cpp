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
    case ECSEngine::Physics::ECollisionShape::CAPSULE:
        return "CAPSULE";
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

CollisionShape CollisionShape::MakeCapsule(vec3 Center, float Radius, float HalfLength)
{
    CollisionShape shape;
    shape.FShapeType = ECollisionShape::CAPSULE;
    shape.FCenter = Center;
    shape.FShapeData.FCapsuleData.Radius = Radius;
    shape.FShapeData.FCapsuleData.HalfLength = HalfLength;
    return shape;
}

float CollisionShape::ComputeMass(float Density) const
{
    switch (FShapeType)
    {
    case ECollisionShape::BOX:
        return Density * FShapeData.FBoxData.FExtents.x * FShapeData.FBoxData.FExtents.y * FShapeData.FBoxData.FExtents.z;
    case ECollisionShape::SPHERE:
        return (4.f / 3.f) * Pi() * FShapeData.FSphereData.Radius * FShapeData.FSphereData.Radius * FShapeData.FSphereData.Radius * Density;
    case ECollisionShape::CAPSULE:
    {
        const float r = FShapeData.FCapsuleData.Radius;
        const float h = FShapeData.FCapsuleData.HalfLength * 2.f;
        const float Vcy = Pi() * r * r * h;
        const float Vhs = (4.f / 3.f) * Pi() * r * r * r; // les 2 hémisphères = 1 sphère
        return Density * (Vcy + Vhs);
    }
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
        break;
    }
    case ECollisionShape::SPHERE:
    {
        inertiaTensor *= 2.f / 5.f * Mass * FShapeData.FSphereData.Radius * FShapeData.FSphereData.Radius;
        break;
    }
    case ECollisionShape::CAPSULE:
    {
        const float r = FShapeData.FCapsuleData.Radius;
        const float h = FShapeData.FCapsuleData.HalfLength * 2.f;

        // Masses des sous-parties
        const float Vcy = Pi() * r * r * h;
        const float Vhs = (4.f / 3.f) * Pi() * r * r * r;
        const float rho = Mass / (Vcy + Vhs);
        const float mcy = rho * Vcy;
        const float mhs = rho * Vhs * 0.5f; // masse d'UN hémisphère

        // Axe principal Y (axe de la capsule)
        const float Iyy = 0.5f * mcy * r * r // cylindre
              + 0.8f * mhs * r * r * 2.f; // 2 hémisphères (4/5 * r²)

        // Axes latéraux X, Z — théorème de Steiner pour les hémisphères
        const float d = h * 0.5f + 3.f * r / 8.f; // distance CM hémisphère → CM capsule
        const float Ixx = mcy * (r * r / 4.f + h * h / 12.f) + 2.f * mhs * ((83.f / 320.f) * r * r + d * d);

        inertiaTensor.FValues[0] = Ixx; // X
        inertiaTensor.FValues[4] = Iyy; // Y (axe capsule)
        inertiaTensor.FValues[8] = Ixx; // Z
        break;
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
    case ECollisionShape::CAPSULE:
    {
        AABB3f OBB(FCenter - vec3(0.f, FShapeData.FCapsuleData.HalfLength, 0.f) - vec3(FShapeData.FCapsuleData.Radius),
              FCenter + vec3(0.f, FShapeData.FCapsuleData.HalfLength, 0.f) + vec3(FShapeData.FCapsuleData.Radius));
        return GeometryHelpers::ComputeAABBFromOBB(OBB, Transform);
    }
    default:
        AssertNotReached();
    }

    return AABB3f();
}

AABB3f CollisionShape::GetLocalAABB() const
{
    switch (FShapeType)
    {
    case ECollisionShape::BOX:
        return AABB3f(FCenter - 0.5f * FShapeData.FBoxData.FExtents, FCenter + 0.5f * FShapeData.FBoxData.FExtents);
    case ECollisionShape::SPHERE:
        return AABB3f(FCenter - vec3(FShapeData.FSphereData.Radius), FCenter + vec3(FShapeData.FSphereData.Radius));
    case ECollisionShape::CAPSULE:
        return AABB3f(FCenter - vec3(0.f, FShapeData.FCapsuleData.HalfLength, 0.f) - vec3(FShapeData.FCapsuleData.Radius),
              FCenter + vec3(0.f, FShapeData.FCapsuleData.HalfLength, 0.f) + vec3(FShapeData.FCapsuleData.Radius));
    default:
        AssertNotReached();
    }
    return AABB3f();
}

float CollisionShape::GetRadius() const
{
    switch (FShapeType)
    {
    case ECollisionShape::SPHERE:
        return FShapeData.FSphereData.Radius;
    case ECollisionShape::CAPSULE:
        return FShapeData.FCapsuleData.Radius;
    }

    AssertNotReached();
    return 0.f;
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

            {
                bool is_selected = (FShapeType == ECollisionShape::CAPSULE);
                if (ImGui::Selectable(GetName(ECollisionShape::CAPSULE).c_str(), is_selected))
                    FShapeType = ECollisionShape::CAPSULE;
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
        case ECSEngine::Physics::ECollisionShape::CAPSULE:
            EDITOR_PROPERTY_WITH_LIMITS("Radius", FShapeData.FCapsuleData.Radius, 0.f, 10000.f);
            EDITOR_PROPERTY_WITH_LIMITS("HalfLength", FShapeData.FCapsuleData.HalfLength, 0.f, 10000.f);
            break;
        default:
            break;
        }
        ImGui::Unindent();
    }
}

} // namespace Physics
} // namespace ECSEngine
